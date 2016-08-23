
#include <cstdint>
#include <iostream>

#include <map>
#include <unordered_map>
#include <string>
#include <time.h>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

struct CountHandler : public osmium::handler::Handler {

    uint64_t count = 0;

    // set timestamp of last update here, to only get updates since then
    const time_t update_from = 1468713600; //"2016-07-17T00:00:00.000Z"

    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>>> map;

    std::unordered_map<std::string, std::string> prevtags;

    void process(osmium::OSMObject& obj, std::string type) {
//std::cout << "n" << obj.id() << "/" << obj.version() << "/" << "/" << std::endl;
        static osmium::object_id_type id_;
        bool update_map = obj.timestamp().seconds_since_epoch() > update_from;
//if (update_map) std::cout << type << "#" << obj.id() << std::endl;// else std::cout << "--#" << count << std::endl;
        if (id_ != obj.id()) { // todo: compare obj's type also
            ++count;
//std::cout << "n#" << count << std::endl;
            prevtags.clear();
            for (const osmium::Tag& tag : obj.tags()) {
                prevtags[tag.key()] = tag.value();
                if (update_map) map[type][tag.key()][tag.value()][obj.timestamp().to_iso().substr(0,10)] += 1;
            }
        } else { // object already existed
//std::cout << "a" << std::endl;
            for(const osmium::Tag& tag : obj.tags()) {
                if (prevtags.count(tag.key()) == 0) {
                    // new tag
                    if (update_map) map[type][tag.key()][tag.value()][obj.timestamp().to_iso().substr(0,10)] += 1;
                    prevtags[tag.key()] = tag.value();
                    //continue;
                }
                //if (prevtags[tag.key()] == tag.value()) continue; // nothing changed
                else if (prevtags[tag.key()] != tag.value()) {
                    // tag changed
                    if (update_map) map[type][tag.key()][prevtags[tag.key()]][obj.timestamp().to_iso().substr(0,10)] -= 1;
                    if (update_map) map[type][tag.key()][tag.value()][obj.timestamp().to_iso().substr(0,10)] += 1;
                    prevtags[tag.key()] = tag.value();
                    //continue;
                }
            }
//std::cout << "b" << std::endl;
            //for( const auto& n : prevtags ) {
            for( auto it = prevtags.begin(); it != prevtags.end(); ) {
                if (!obj.tags().has_key(it->first.data())) {
                    // removed tag
                    if (update_map) map[type][it->first][it->second][obj.timestamp().to_iso().substr(0,10)] -= 1;
                    it = prevtags.erase(it);
                } else ++it;
            }
//std::cout << "c" << std::endl;
        }
        id_ = obj.id();
    }

    void node(osmium::Node& obj) {
      process(obj, "node");
    }
    void way(osmium::Way& obj) {
      process(obj, "way");
    }
    void relation(osmium::Relation& obj) {
      process(obj, "relation");
    }

};


int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    osmium::io::File infile(argv[1], ".pbf");
    osmium::io::Reader reader(infile, osmium::osm_entity_bits::all);

    CountHandler handler;
    osmium::apply(reader, handler);
    reader.close();

    std::cerr << "processed objects count: "     << handler.count << "\n";

    std::string type,key,value;
    for( const auto& t : handler.map ) {
    for( const auto& k : t.second ) {
    for( const auto& v : k.second ) {
    for( const auto& d : v.second ) {
        if (d.second == 0) continue;
        type = t.first;
        key = k.first;
        value = v.first;
        std::replace( key.begin(), key.end(), '\r', '?');
        std::replace( key.begin(), key.end(), '\n', '?');
        std::replace( value.begin(), value.end(), '\r', '?');
        std::replace( value.begin(), value.end(), '\n', '?');
        // todo: replace " with ""
        std::cout << d.first << "T00:00:00.000Z," << type << ",\"" << key << "\",\"" << value << "\"," << d.second << "\n";
    }}}}
}
