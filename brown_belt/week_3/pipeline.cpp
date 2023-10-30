#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
    string from;
    string to;
    string body;
};


class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        // только первому worker-у в пайплайне нужно это имплементировать
        throw logic_error("Unimplemented");
    }

protected:
    // реализации должны вызывать PassOn, чтобы передать объект дальше
    // по цепочке обработчиков
    void PassOn(unique_ptr<Email> email) const {
        if (next_) {
            next_->Process(move(email));
        }
    }

public:
    void SetNext(unique_ptr<Worker> next) {
        next_ = move(next);
    }

private:
    unique_ptr<Worker> next_;
};


class Reader : public Worker {
public:
    Reader(istream& in)
        : input_stream_(in)
    {}

    void Process(unique_ptr<Email> ) override {

    }

    void Run() override {
        while(true)
        {
            unique_ptr<Email> email = make_unique<Email>();
            getline(input_stream_, email->from);
            getline(input_stream_, email->to);
            getline(input_stream_, email->body);
            if (input_stream_) {
                PassOn(move(email));
            } else {
                break;
            }
        }
    }

private:
    istream& input_stream_;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

public:
    Filter(Function filter)
        : filter_(move(filter))
    {}

    void Process(unique_ptr<Email> email) override {
        if (filter_(*email.get())) {
            PassOn(move(email));
        }
    }
private:
    Function filter_;
};


class Copier : public Worker {
public:
    Copier(string recipient)
        : recipient_(recipient)
    {}

    void Process(unique_ptr<Email> email) override {
        if (email->to != recipient_) {
            unique_ptr<Email> new_email = make_unique<Email>();
            new_email->to = recipient_;
            new_email->from = email->from;
            new_email->body = email->body;
            PassOn(move(email));
            PassOn(move(new_email));
        } else {
            PassOn(move(email));
        }
    }

private:
    string recipient_;
};


class Sender : public Worker {
public:
    Sender(ostream& out)
        : out_stream_(out)
    {}

    void Process(unique_ptr<Email> email) override {
        out_stream_ << email->from << endl
                    << email->to   << endl
                    << email->body << endl;
        PassOn(move(email));
    }

private:
    ostream& out_stream_;
};


// реализуйте класс
class PipelineBuilder {
public:
    // добавляет в качестве первого обработчика Reader
    explicit PipelineBuilder(istream& in) {
        head_worker_ = make_unique<Reader>(in);
        last_worker_ptr_ = head_worker_.get();
    }

    // добавляет новый обработчик Filter
    PipelineBuilder& FilterBy(Filter::Function filter) {
        unique_ptr<Worker> filter_worker = make_unique<Filter>(filter);
        Worker* ptr = filter_worker.get();
        last_worker_ptr_->SetNext(move(filter_worker));
        last_worker_ptr_ = ptr;
        return *this;
    }

    // добавляет новый обработчик Copier
    PipelineBuilder& CopyTo(string recipient) {
        unique_ptr<Worker> copy_worker = make_unique<Copier>(recipient);
        Worker* ptr = copy_worker.get();
        last_worker_ptr_->SetNext(move(copy_worker));
        last_worker_ptr_ = ptr;
        return *this;
    }

    // добавляет новый обработчик Sender
    PipelineBuilder& Send(ostream& out) {
        unique_ptr<Worker> send_worker = make_unique<Sender>(out);
        Worker* ptr = send_worker.get();
        last_worker_ptr_->SetNext(move(send_worker));
        last_worker_ptr_ = ptr;
        return *this;
    }

    // возвращает готовую цепочку обработчиков
    unique_ptr<Worker> Build() {
        return move(head_worker_);
    }

private:
    unique_ptr<Worker> head_worker_;
    Worker* last_worker_ptr_;
};


void TestSanity() {
    string input = (
                "erich@example.com\n"
                "richard@example.com\n"
                "Hello there\n"

                "erich@example.com\n"
                "ralph@example.com\n"
                "Are you sure you pressed the right button?\n"

                "ralph@example.com\n"
                "erich@example.com\n"
                "I do not make mistakes of that kind\n"
                );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
    });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
                "erich@example.com\n"
                "richard@example.com\n"
                "Hello there\n"

                "erich@example.com\n"
                "ralph@example.com\n"
                "Are you sure you pressed the right button?\n"

                "erich@example.com\n"
                "richard@example.com\n"
                "Are you sure you pressed the right button?\n"
                );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

void TestOneEmail() {
    string input = (
                "erich@example.com\n"
                "richard@example.com\n"
                "Hello there\n"
                );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
                "erich@example.com\n"
                "richard@example.com\n"
                "Hello there\n"
                );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    RUN_TEST(tr, TestOneEmail);
    return 0;
}
