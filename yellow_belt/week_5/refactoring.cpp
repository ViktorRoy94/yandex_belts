#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

class Person {
protected:
    Person(const string& name, const string& type) :
        name_(name), type_(type)
    {
    }

    ostream& Log() {
        return cout << type_ << ": " << name_;
    }

public:
    virtual void Walk(const string& destination) {
        Log() << " walks to: " << destination << endl;
    }

    const string name_;
    const string type_;
};


class Student : public Person{
public:
    Student(const string& name, const string& favouriteSong) :
        Person(name, "Student"), favouriteSong_(favouriteSong)
    {
    }

    void Learn() {
        Log() << " learns" << endl;
    }

    void Walk(const string& destination) override{
        Log() << " walks to: " << destination << endl;
        Log() << " sings a song: " << favouriteSong_ << endl;
    }

    void SingSong() {
        Log() << " sings a song: " << favouriteSong_ << endl;
    }

protected:
    const string favouriteSong_;
};


class Teacher : public Person{
public:
    Teacher(const string& name, const string& subject) :
        Person(name, "Teacher"), subject_(subject)
    {
    }

    void Teach() {
        Log() << " teaches: " << subject_ << endl;
    }

protected:
    const string subject_;
};


class Policeman : public Person {
public:
    Policeman(const string& name) :
        Person(name, "Policemen")
    {
    }

    void Check(Person& p) {
        Log() << " checks " << p.type_ << ". " << p.type_ << "'s name is: " << p.name_ << endl;
    }
};

void VisitPlaces(Person& person, const vector<string>& places) {
    for (auto p : places) {
        person.Walk(p);
    }
}

int main() {
    Teacher t("Jim", "Math");
    Student s("Ann", "We will rock you");
    Policeman p("Bob");

    VisitPlaces(t, {"Moscow", "London"});
    p.Check(s);
    VisitPlaces(s, {"Moscow", "London"});
    return 0;
}
