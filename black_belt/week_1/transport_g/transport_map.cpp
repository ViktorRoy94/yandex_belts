#include "transport_map.h"

TransportMap::TransportMap(const Descriptions::StopsDict& stops_dict,
                           const Descriptions::BusesDict& buses_dict,
                           const Json::Dict& routing_settings_json)
    : render_settings_(MakeRenderSettings(routing_settings_json)) {

    ComputeBorders(stops_dict);

    int bus_color_num = 0;
    for (const auto& [_, bus] : buses_dict) {
        Svg::Polyline line;
        line.SetStrokeColor(render_settings_.color_palette[bus_color_num])
            .SetStrokeWidth(render_settings_.line_width)
            .SetStrokeLineCap("round")
            .SetStrokeLineJoin("round");
        for (const string& stop_name : bus->stops) {
            Sphere::Point real_point = stops_dict.at(stop_name)->position;
            line.AddPoint(ConvertCoords(real_point));
        }
        bus_color_num = (bus_color_num + 1) % render_settings_.color_palette.size();
        svg_map_.Add(move(line));
    }

    for (const auto& [_, stop] : stops_dict) {
        Svg::Circle circle;
        circle.SetCenter(ConvertCoords(stop->position))
              .SetRadius(render_settings_.stop_radius)
              .SetFillColor("white");
        svg_map_.Add(move(circle));
    }

    for (const auto& [_, stop] : stops_dict) {
        Svg::Text text_background;
        Svg::Text text;

        text_background.SetPoint(ConvertCoords(stop->position))
                       .SetOffset(render_settings_.stop_label_offset)
                       .SetFontSize(render_settings_.stop_label_font_size)
                       .SetFontFamily("Verdana")
                       .SetData(stop->name)
                       .SetFillColor(render_settings_.underlayer_color)
                       .SetStrokeColor(render_settings_.underlayer_color)
                       .SetStrokeWidth(render_settings_.underlayer_width)
                       .SetStrokeLineCap("round")
                       .SetStrokeLineJoin("round");

        text.SetPoint(ConvertCoords(stop->position))
            .SetOffset(render_settings_.stop_label_offset)
            .SetFontSize(render_settings_.stop_label_font_size)
            .SetFontFamily("Verdana")
            .SetFillColor("black")
            .SetData(stop->name);

        svg_map_.Add(move(text_background));
        svg_map_.Add(move(text));
    }
}

std::string TransportMap::RenderMap() const {
    ostringstream output;
    svg_map_.Render(output);
    cout << output.str() << endl;
    return output.str();
}

Svg::Color ParseJsonColor(const Json::Node& node) {
    try {
        return node.AsString();
    } catch (bad_variant_access const& ex) {
        const auto& arr = node.AsArray();
        Svg::Rgb rgb = {static_cast<uint8_t>(arr[0].AsInt()),
                        static_cast<uint8_t>(arr[1].AsInt()),
                        static_cast<uint8_t>(arr[2].AsInt())};
        if (arr.size() == 4) {
            return Svg::Rgba({rgb.red, rgb.green, rgb.blue, arr[3].AsDouble()});
        } else {
            return rgb;
        }
    }
}

vector<Svg::Color> ParseJsonColors(const Json::Array& json_array) {
    vector<Svg::Color> colors(json_array.size());
    for (size_t i = 0; i < json_array.size(); i++) {
        colors[i] = ParseJsonColor(json_array[i]);
    }
    return colors;
}

TransportMap::RenderSettings TransportMap::MakeRenderSettings(const Json::Dict& json) {
    return {json.at("width").AsDouble(),
            json.at("height").AsDouble(),
            json.at("padding").AsDouble(),
            json.at("stop_radius").AsDouble(),
            json.at("line_width").AsDouble(),
            json.at("stop_label_font_size").AsInt(),
           {json.at("stop_label_offset").AsArray()[0].AsDouble(),
            json.at("stop_label_offset").AsArray()[1].AsDouble()},
            ParseJsonColor(json.at("underlayer_color")),
            json.at("underlayer_width").AsDouble(),
            ParseJsonColors(json.at("color_palette").AsArray())
        };
}

void TransportMap::ComputeBorders(const Descriptions::StopsDict& stops_dict) {
    min_sphere_point_ = stops_dict.begin()->second->position;
    max_sphere_point_ = stops_dict.begin()->second->position;
    for (const auto& [_, stop] : stops_dict) {
        if (stop->position.latitude < min_sphere_point_.latitude) {
            min_sphere_point_.latitude = stop->position.latitude;
        }
        if (stop->position.latitude > max_sphere_point_.latitude) {
            max_sphere_point_.latitude = stop->position.latitude;
        }
        if (stop->position.longitude < min_sphere_point_.longitude) {
            min_sphere_point_.longitude = stop->position.longitude;
        }
        if (stop->position.longitude > max_sphere_point_.longitude) {
            max_sphere_point_.longitude = stop->position.longitude;
        }
    }
    double width_zoom_coef  = 0;
    double height_zoom_coef = 0;

    if (max_sphere_point_.longitude != min_sphere_point_.longitude) {
        width_zoom_coef  = (render_settings_.width - 2 * render_settings_.padding) /
                           (max_sphere_point_.longitude - min_sphere_point_.longitude);
    }
    if (max_sphere_point_.latitude != min_sphere_point_.latitude) {
        height_zoom_coef = (render_settings_.height - 2 * render_settings_.padding) /
                           (max_sphere_point_.latitude - min_sphere_point_.latitude);
    }
    if (width_zoom_coef > 0 && height_zoom_coef > 0) {
        zoom_coef_ = min(width_zoom_coef, height_zoom_coef);
    } else {
        zoom_coef_ = max(width_zoom_coef, height_zoom_coef);
    }

}

Svg::Point TransportMap::ConvertCoords(const Sphere::Point& real_point) {
    return {(real_point.longitude - min_sphere_point_.longitude) * zoom_coef_ + render_settings_.padding,
            (max_sphere_point_.latitude - real_point.latitude)   * zoom_coef_ + render_settings_.padding};
}

void TransportMap::FillMapByPaths(const Descriptions::BusesDict& buses_dict)
{

}