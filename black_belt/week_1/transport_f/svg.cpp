#include "test_runner.h"

#include <iostream>
#include <memory>
#include <variant>

using namespace std;

namespace Svg
{

struct Point
{
    double x = 0.0;
    double y = 0.0;
};

struct Rgb
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Color
{
    Color() = default;
    Color(const char* color);
    Color(std::string color);
    Color(Rgb color);

    std::string text = "none";
};
const Color NoneColor;

std::ostream& operator << (std::ostream& os, const Point& c);
std::ostream& operator << (std::ostream& os, const Rgb& c);
std::ostream& operator << (std::ostream& os, const Color& c);
bool operator == (const Point& lhs, const Point& rhs);
bool operator == (const Rgb& lhs, const Rgb& rhs);
bool operator == (const Color& lhs, const Color& rhs);

class ObjectBase
{
public:
    virtual void Render(std::ostream& out = std::cout) const = 0;
    virtual ~ObjectBase() = default;
};

template<typename Owner>
class PathProps
{
public:
    PathProps();
    void RenderAttrs(ostream& out) const;
    Owner& SetFillColor(const Color& color);
    Owner& SetStrokeColor(const Color& color);
    Owner& SetStrokeWidth(double width);
    Owner& SetStrokeLineCap(const std::string& linecap);
    Owner& SetStrokeLineJoin(const std::string& linejoin);
    virtual ~PathProps() = default;
protected:
    Color fill_color_;
    Color stroke_color_;
    double stroke_width_;
    std::optional<std::string> stroke_linecap_;
    std::optional<std::string> stroke_linejoin_;

    Owner& AsOwner();
};

class Circle : public ObjectBase, public PathProps<Circle>
{
public:
    Circle();
    Circle& SetCenter(Point p);
    Circle& SetRadius(double r);
    virtual void Render(std::ostream& out = std::cout) const override;

private:
    Point center_;
    double r_;
};

class Polyline : public ObjectBase, public PathProps<Polyline>
{
public:
    Polyline();
    Polyline& AddPoint(Point p);
    virtual void Render(std::ostream& out = std::cout) const override;
private:
    std::vector<Point> line_;
};

class Text : public ObjectBase, public PathProps<Text>
{
public:
    Text();
    Text& SetPoint(Point p);
    Text& SetOffset(Point p);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(const std::string& font_name);
    Text& SetData(const std::string& text_data);
    virtual void Render(std::ostream& out = std::cout) const  override;
private:
    Point position_;
    Point offset_;
    uint32_t font_size_;
    std::string font_name_;
    std::string text_data_;
};

using Object = std::variant<Circle, Polyline, Text>;
class Document
{
public:
    void Add(Object obj);
    void Render(std::ostream& out = std::cout);

private:
    std::vector<Object> objects_;
};

/////////////////////////////////////////////////////////////
/// Color methods
/////////////////////////////////////////////////////////////

Color::Color(const char* color)
{
    text = std::string(color);
}
Color::Color(std::string color)
{
    text = move(color);
}
Color::Color(Rgb color)
{
    text = "rgb(" + std::to_string(color.red) + "," +
                    std::to_string(color.green) + "," +
                    std::to_string(color.blue) + ")";
}

std::ostream& operator << (std::ostream& os, const Color& c)
{
    return os << c.text;
}

bool operator == (const Color& lhs, const Color& rhs)
{
    return lhs.text == rhs.text;
}

/////////////////////////////////////////////////////////////
/// PathProps methods
/////////////////////////////////////////////////////////////

template<typename Owner>
PathProps<Owner>::PathProps()
    : fill_color_(Svg::NoneColor)
    , stroke_color_(Svg::NoneColor)
    , stroke_width_(1.0) {}

template<typename Owner>
Owner& PathProps<Owner>::SetFillColor(const Color& color)
{
    fill_color_ = color;
    return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeColor(const Color& color)
{
    stroke_color_ = color;
    return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeWidth(double width)
{
    stroke_width_ = width;
    return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeLineCap(const std::string& linecap)
{
    stroke_linecap_ = linecap;
    return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeLineJoin(const std::string& linejoin)
{
    stroke_linejoin_ = linejoin;
    return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::AsOwner()
{
    return static_cast<Owner&>(*this);
}
template <typename Owner>
void PathProps<Owner>::RenderAttrs(ostream& out) const
{
    out << "fill=\"" << fill_color_ << "\" ";
    out << "stroke=\"" << stroke_color_ << "\" ";
    out << "stroke-width=\"" << stroke_width_ << "\" ";
    if (stroke_linecap_) {
        out << "stroke-linecap=\"" << *stroke_linecap_ << "\" ";
    }
    if (stroke_linejoin_) {
        out << "stroke-linejoin=\"" << *stroke_linejoin_ << "\" ";
    }
}

/////////////////////////////////////////////////////////////
/// Circle methods
/////////////////////////////////////////////////////////////

Circle::Circle()
    : ObjectBase()
    , center_({0.0, 0.0})
    , r_(1.0) {}

Circle& Circle::SetCenter(Point p)
{
    center_ = p;
    return *this;
}
Circle& Circle::SetRadius(double r)
{
    r_ = r;
    return *this;
}
void Circle::Render(std::ostream& out) const
{
    out << "<circle "
        << "cx=\""            << center_.x     << "\" "
        << "cy=\""            << center_.y     << "\" "
        << "r=\""             << r_            << "\" ";
    PathProps::RenderAttrs(out);
    out << "/>";
}


/////////////////////////////////////////////////////////////
/// Polyline methods
/////////////////////////////////////////////////////////////

Polyline::Polyline()
    : ObjectBase() {}

Polyline& Polyline::AddPoint(Point p)
{
    line_.push_back(move(p));
    return *this;
}
void Polyline::Render(std::ostream& out) const
{
    out << "<polyline ";

    out << "points=\"";
    for (const Point& p : line_) {
        out << p.x << "," << p.y << " ";
    }
    out << "\" ";
    PathProps::RenderAttrs(out);
    out << "/>";
}

/////////////////////////////////////////////////////////////
/// Text methods
/////////////////////////////////////////////////////////////

Text::Text()
    : ObjectBase()
    , position_({})
    , offset_({})
    , font_size_(1) {}

Text& Text::SetPoint(Point p)
{
    position_ = p;
    return *this;
}
Text& Text::SetOffset(Point p)
{
    offset_ = p;
    return *this;
}
Text& Text::SetFontSize(uint32_t size)
{
    font_size_ = size;
    return *this;
}
Text& Text::SetFontFamily(const std::string& font_name)
{
    font_name_ = font_name;
    return *this;
}
Text& Text::SetData(const std::string& text_data)
{
    text_data_ = text_data;
    return *this;
}

void Text::Render(std::ostream& out) const
{
    out << "<text "
        << "x=\""         << position_.x << "\" "
        << "y=\""         << position_.y << "\" "
        << "dx=\""        << offset_.x   << "\" "
        << "dy=\""        << offset_.y   << "\" "
        << "font-size=\"" << font_size_  << "\" ";

    if (!font_name_.empty()) {
        out << "font-family=\"" << font_name_ << "\" ";
    }
    PathProps::RenderAttrs(out);
    out << ">";
    out << text_data_ << "</text>";
}

/////////////////////////////////////////////////////////////
/// Document methods
/////////////////////////////////////////////////////////////

void Document::Add(Object obj)
{
    objects_.push_back(move(obj));
}
void Document::Render(std::ostream& out)
{
    out << R"(<?xml version="1.0" encoding="UTF-8" ?>)"
        << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";

    for (const Object& obj : objects_) {
        visit([&out](const auto& obj) {
            obj.Render(out);
        }, obj);
    }
    out << "</svg>";
}

}

void TestPoint() {
    {
        Svg::Point p;
        ASSERT_EQUAL(p.x, 0.0);
        ASSERT_EQUAL(p.y, 0.0);
    }
    {
        Svg::Point p;
        p.x = 10;
        p.y = 15;
        ASSERT_EQUAL(p.x, 10);
        ASSERT_EQUAL(p.y, 15);
    }
    {
        Svg::Point p = {10, 15};
        ASSERT_EQUAL(p.x, 10);
        ASSERT_EQUAL(p.y, 15);
    }
}
void TestRgb() {
    {
        Svg::Rgb rgb;
        ASSERT_EQUAL(rgb.red, 0);
        ASSERT_EQUAL(rgb.green, 0);
        ASSERT_EQUAL(rgb.blue, 0);
    }
    {
        Svg::Rgb rgb;
        rgb.red = 1;
        rgb.green = 2;
        rgb.blue = 3;
        ASSERT_EQUAL(rgb.red, 1);
        ASSERT_EQUAL(rgb.green, 2);
        ASSERT_EQUAL(rgb.blue, 3);
    }
    {
        Svg::Rgb rgb = {1, 2, 3};
        ASSERT_EQUAL(rgb.red, 1);
        ASSERT_EQUAL(rgb.green, 2);
        ASSERT_EQUAL(rgb.blue, 3);
    }
}
void TestColor() {
    {
        ASSERT_EQUAL(Svg::NoneColor.text, "none");
    }
    {
        Svg::Color c("white");
        ASSERT_EQUAL(c.text, "white");
    }
    {
        Svg::Color c(string("black"));
        ASSERT_EQUAL(c.text, "black");
    }
    {
        Svg::Color c(Svg::Rgb{255, 255, 255});
        ASSERT_EQUAL(c.text, "rgb(255,255,255)");
    }
}

void TestCircle() {
    {
        ostringstream output;
        Svg::Circle{}.Render(output);
        ASSERT_EQUAL(output.str(), R"(<circle cx="0" cy="0" r="1" fill="none" stroke="none" stroke-width="1" />)");
    }
    {
        ostringstream output;
        Svg::Circle{}
          .SetFillColor("white")
          .SetRadius(6)
          .SetCenter(Svg::Point{50, 50})
          .Render(output);
        ASSERT_EQUAL(output.str(), R"(<circle cx="50" cy="50" r="6" fill="white" stroke="none" stroke-width="1" />)");
    }
}

void TestPolyline() {
    {
        ostringstream output;
        Svg::Polyline{}.Render(output);
        ASSERT_EQUAL(output.str(), R"(<polyline points="" fill="none" stroke="none" stroke-width="1" />)");
    }
    {
        ostringstream output;
        Svg::Polyline{}
          .SetStrokeColor(Svg::Rgb{140, 198, 63})
          .SetStrokeWidth(16)
          .SetStrokeLineCap("round")
          .AddPoint({50, 50})
          .AddPoint({250, 250})
          .Render(output);
        ASSERT_EQUAL(output.str(), R"~(<polyline points="50,50 250,250 " fill="none" stroke="rgb(140,198,63)" stroke-width="16" stroke-linecap="round" />)~");
    }
}

void TestText() {
    {
        ostringstream output;
        Svg::Text{}.Render(output);
        ASSERT_EQUAL(output.str(), R"(<text x="0" y="0" dx="0" dy="0" font-size="1" fill="none" stroke="none" stroke-width="1" ></text>)");
    }
    {
        ostringstream output;
        Svg::Text{}
          .SetPoint({50, 50})
          .SetOffset({10, -10})
          .SetFontSize(20)
          .SetFontFamily("Verdana")
          .SetFillColor("black")
          .SetData("C")
          .Render(output);
        ASSERT_EQUAL(output.str(), R"~(<text x="50" y="50" dx="10" dy="-10" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >C</text>)~");
    }
}

void TestPipeline() {
    Svg::Document svg;
    svg.Add(
        Svg::Polyline{}
        .SetStrokeColor(Svg::Rgb{140, 198, 63})  // soft green
        .SetStrokeWidth(16)
        .SetStrokeLineCap("round")
        .AddPoint({50, 50})
        .AddPoint({250, 250})
    );

    for (const auto point : {Svg::Point{50, 50}, Svg::Point{250, 250}}) {
      svg.Add(
          Svg::Circle{}
          .SetFillColor("white")
          .SetRadius(6)
          .SetCenter(point)
      );
    }

    svg.Add(
        Svg::Text{}
        .SetPoint({50, 50})
        .SetOffset({10, -10})
        .SetFontSize(20)
        .SetFontFamily("Verdana")
        .SetFillColor("black")
        .SetData("C")
    );
    svg.Add(
        Svg::Text{}
        .SetPoint({250, 250})
        .SetOffset({10, -10})
        .SetFontSize(20)
        .SetFontFamily("Verdana")
        .SetFillColor("black")
        .SetData("C++")
    );

    ostringstream output;
    svg.Render(output);
    ASSERT_EQUAL(output.str(), R"~(<?xml version="1.0" encoding="UTF-8" ?><svg xmlns="http://www.w3.org/2000/svg" version="1.1"><polyline points="50,50 250,250 " fill="none" stroke="rgb(140,198,63)" stroke-width="16" stroke-linecap="round" /><circle cx="50" cy="50" r="6" fill="white" stroke="none" stroke-width="1" /><circle cx="250" cy="250" r="6" fill="white" stroke="none" stroke-width="1" /><text x="50" y="50" dx="10" dy="-10" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >C</text><text x="250" y="250" dx="10" dy="-10" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >C++</text></svg>)~");
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestPoint);
    RUN_TEST(tr, TestRgb);
    RUN_TEST(tr, TestColor);
    RUN_TEST(tr, TestCircle);
    RUN_TEST(tr, TestPolyline);
    RUN_TEST(tr, TestText);
    RUN_TEST(tr, TestPipeline);
#else
#endif
    return 0;
}
