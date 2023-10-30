#pragma once

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

struct Rgba
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double alpha = 0.0;
};

using Color = variant<monostate, string, Rgb, Rgba>;
const Color NoneColor{};

void RenderColor(ostream& out, monostate);
void RenderColor(ostream& out, const string& value);
void RenderColor(ostream& out, Rgb rgb);
void RenderColor(ostream& out, Rgba rgba);
std::ostream& operator <<(std::ostream& out, const Color& color);

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
    void Render(std::ostream& out = std::cout) const;

private:
    std::vector<Object> objects_;
};

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
void TestPoint();
void TestRgb();
void TestColor();
void TestCircle();
void TestPolyline();
void TestText();
void TestPipeline();

}