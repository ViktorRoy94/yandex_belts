#pragma once

#include "descriptions.h"
#include "json.h"
#include "svg.h"
#include "sphere.h"

#include <memory>
#include <unordered_map>
#include <vector>

class TransportMap {
public:
    TransportMap(const Descriptions::StopsDict& stops_dict,
                 const Descriptions::BusesDict& buses_dict,
                 const Json::Dict& routing_settings_json);

    std::string RenderMap() const;

private:
    struct RenderSettings {
        double width;
        double height;
        double padding;
        double stop_radius;
        double line_width;
        int stop_label_font_size;
        Svg::Point stop_label_offset;
        Svg::Color underlayer_color;
        double underlayer_width;
        std::vector<Svg::Color> color_palette;
    };
    static RenderSettings MakeRenderSettings(const Json::Dict& json);

    void ComputeBorders(const Descriptions::StopsDict& stops_dict);
    Svg::Point ConvertCoords(const Sphere::Point& real_point);

    void FillMapByPaths(const Descriptions::BusesDict& buses_dict);
    void FillMapByStops(const Descriptions::StopsDict& stops_dict);

    RenderSettings render_settings_;
    Svg::Document svg_map_;

    Sphere::Point min_sphere_point_;
    Sphere::Point max_sphere_point_;
    double zoom_coef_;
};
