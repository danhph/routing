#include <optional>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <io2d.h>
#include "route_model.h"
#include "render.h"
#include "route_planner.h"

using namespace std::experimental;

static std::optional<std::vector<std::byte>> ReadFile(const std::string &path)
{   
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if( !is )
        return std::nullopt;
    
    auto size = is.tellg();
    std::vector<std::byte> contents(size);    
    
    is.seekg(0);
    is.read((char*)contents.data(), size);

    if( contents.empty() )
        return std::nullopt;
    return std::move(contents);
}

int main(int argc, const char **argv)
{    
    std::string osm_data_file = "map.osm";
    std::string traffic_data_file = "traffic.xml";

    std::cout << "Reading OpenStreetMap data from the following file: " <<  osm_data_file << std::endl;
    auto data = ReadFile(osm_data_file);
    if( !data )
    {
        std::cout << "Failed to read." << std::endl;
        return 1;
    }
    std::vector<std::byte> osm_data = std::move(*data);
    
    // start and end node
    float start_x, start_y, end_x, end_y;

    // get the user input for start node: (start_x, start_y) and end node: (end_x, end_y)
    std::cout << "The map coordinates start at (0,0) in the lower left corner and end at (100,100) ine the upper right corner."<<std::endl;
    // std::cout << "Enter a value for start x between 0-100: ";
    // std::cin >> start_x;
    // std::cout << "Enter a value for start y between 0-100: ";
    // std::cin >> start_y;
    // std::cout << "Enter a value for end x between 0-100: ";
    // std::cin >> end_x;
    // std::cout << "Enter a value for end y between 0-100: ";
    // std::cin >> end_y;
	start_x = 16;
	start_y = 92;
	
	// end_x = 63;
	// end_y = 87;
	end_x = 67;
	end_y = 53;

    // Build Model.
    RouteModel model{osm_data};

	// Load traffic data 
    std::cout << "Reading traffic data from the following file: " <<  traffic_data_file << std::endl;
    data = ReadFile(traffic_data_file);
    if(data)
    {
        std::vector<std::byte> traffic_data = std::move(*data);
    	model.LoadTrafficData(traffic_data);
    }

    // Perform search and render results.
    RoutePlanner route_planner{model, start_x, start_y, end_x, end_y};
    route_planner.AStarSearch();

    // printout the length of the path
    std::cout<<"The length of the path = "<<route_planner.GetDistance()<<" meters"<<std::endl;
    Render render{model};

    auto display = io2d::output_surface{800, 800, io2d::format::argb32, io2d::scaling::none, io2d::refresh_style::fixed, 30};
    display.size_change_callback([](io2d::output_surface& surface){
        surface.dimensions(surface.display_dimensions());
    });
    display.draw_callback([&](io2d::output_surface& surface){
        render.Display(surface);
    });
    display.begin_show();
}
