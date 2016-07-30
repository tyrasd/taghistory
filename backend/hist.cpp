/*

  This is a small tool that counts the number of nodes, ways, and relations in
  the input file.

  The code in this example file is released into the Public Domain.

*/

#include <cstdint>
#include <iostream>

#include <map>
#include <unordered_map>
#include <string>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

struct CountHandler : public osmium::handler::Handler {

    uint64_t nodes = 0;
    
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> map;

    std::unordered_map<std::string, std::string> prevtags;

    void way(osmium::Way& obj) {
//std::cout << "n" << obj.id() << "/" << obj.version() << "/" << "/" << std::endl;
        static osmium::object_id_type id_;
        if (id_ != obj.id()) {
            ++nodes;
//std::cout << "n#" << nodes << std::endl;
            prevtags.clear();
            for (const osmium::Tag& tag : obj.tags()) {
                prevtags[tag.key()] = tag.value();
                map[tag.key()][tag.value()][obj.timestamp().to_iso().substr(0,10)] += 1;
            }
        } else { // object already existed
//std::cout << "a" << std::endl;
            for(const osmium::Tag& tag : obj.tags()) {
                if (prevtags.count(tag.key()) == 0) {
                    // new tag
                    map[tag.key()][tag.value()][obj.timestamp().to_iso().substr(0,10)] += 1;
                    prevtags[tag.key()] = tag.value();
                    //continue;
                } 
                //if (prevtags[tag.key()] == tag.value()) continue; // nothing changed
                else if (prevtags[tag.key()] != tag.value()) {
                    // tag changed
                    map[tag.key()][prevtags[tag.key()]][obj.timestamp().to_iso().substr(0,10)] -= 1;
                    map[tag.key()][tag.value()][obj.timestamp().to_iso().substr(0,10)] += 1;
                    prevtags[tag.key()] = tag.value();
                    //continue;
                }
            }
//std::cout << "b" << std::endl;
            //for( const auto& n : prevtags ) {
            for( auto it = prevtags.begin(); it != prevtags.end(); ) {
                if (!obj.tags().has_key(it->first.data())) {
                    // removed tag
                    map[it->first][it->second][obj.timestamp().to_iso().substr(0,10)] -= 1;
                    it = prevtags.erase(it);
                } else ++it;
            }
//std::cout << "c" << std::endl;
        }
        id_ = obj.id();
    }

};


int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    osmium::io::File infile(argv[1], ".pbf");
    osmium::io::Reader reader(infile, osmium::osm_entity_bits::way);

    CountHandler handler;
    osmium::apply(reader, handler);
    reader.close();

    std::cerr << "Nodes: "     << handler.nodes << "\n";

    std::string key,value;
    for( const auto& k : handler.map ) {
    for( const auto& v : k.second ) {
    for( const auto& d : v.second ) {
        if (d.second == 0) continue;
        key = k.first;
        value = v.first;
        std::replace( key.begin(), key.end(), '\r', '?');
        std::replace( key.begin(), key.end(), '\n', '?');
        std::replace( value.begin(), value.end(), '\r', '?');
        std::replace( value.begin(), value.end(), '\n', '?');
        std::cout << d.first << "T00:00:00.000Z,way,\"" << key << "\",\"" << value << "\"," << d.second << "\n";
    }}}
}

