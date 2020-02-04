OSM Tag Usage Analysis
======================

Generates graphs of the usage of arbitrary [OSM](//osm.org) [tags](//wiki.openstreetmap.org/wiki/Tags) over time (with daily granularity) by number of OSM objects. Read more about OSM tags and this tool in my blog [article](http://www.openstreetmap.org/user/tyr_asd/diary/39402).

<img src="https://wiki.openstreetmap.org/w/images/a/a6/Highway%3Dford_vs_ford%3Dyes.svg" width="700">

Warning: The output is (currently) only given in terms of numbers (counts) of OSM objects! Similarly to some of the statistics produced by [taginfo](//taginfo.openstreetmap.org), it is subject to the same limitations, most notably the effect that one cannot directly compare the number of tags used for different linear and polygonal features such as roads, land cover, etc. because such features are typically divided up into many OSM objects of different sizes. For example, an existing road may be divided up into two pieces when a new turn restrictions is added, resulting in that the count of each of the tags used on the road (even obsolete ones) is increased by one in the OSM database. That means that one needs to pay close attention when comparing tags that are typically used on such features, even when one's comparing subtags that are typically used on the same kind of parent object (e.g. different values of the `highway` tag).

Technicalities
--------------

A simple osmium script (see [`/backend/`](https://github.com/tyrasd/taghistory/tree/master/backend)) scans through a planet history dump and aggregates daily net differences of all used tags by osm object type (node/way/relation). This takes care of created and deleted objects as well as tag modifications in between different versions of an object.

This data is then stored into an sqlite database and is exposed by a simple REST API.

API
---

Data returned from the taghistory API described below is available under the terms of the [ODbL 1.0](http://opendatacommons.org/licenses/odbl/1.0/) license and copyright © [OpenStreetMap contributors](https://www.openstreetmap.org/copyright).

### `GET /<type>/<key>/<value>[?format=<format>]`

Gets the osm object count history for the given tag (`key`, `value`) of the given OSM object type (`type`). `type` can be `***` to search for any object type.

Returns a JSON array of objects containing `date`, `delta` and `count` fields indicating the net change (`delta`) of the respective tag usage on a particular `date`. `count` contains a running cummulative sum of the object counts for convenience.

The optional GET-parameter `format` can be one of `json` (default value if omitted, see the example below) or `csv` for a downloadable csv file.

Example: `http://taghistory.raifer.tech/relation/amenity/drinking_water`

```json
[
  {"date":"2010-08-03T00:00:00.000Z","delta":1,"count":1},
  {"date":"2010-08-28T00:00:00.000Z","delta":3,"count":4},
  …,
  {"date":"2014-10-28T00:00:00.000Z","delta":-3,"count":0}
]
```

### `GET /<type>/<key>[?format=<format>]`

Same as above, but matches any tag with the given key.

todos
-----

* implement regular (e.g. daily) data updates
* for linear and polygonal objects: use length or area as a metric instead of object count

see also
--------

* [ohsome dashboard](https://ohsome.org/apps/dashboard) – Provides detailed statistics about the historical development of OpenStreetMap data
* [taginfo](https://github.com/joto/taginfo) – Brings together information about OpenStreetMap tags and makes it searchable and browsable
* [osm-analytics](https://github.com/hotosm/osm-analytics) – Analyse interactively how and when specific OpenStreetMap features are mapped in specific regions
* [*Stats* page](http://wiki.openstreetmap.org/wiki/Stats) on the OSM Wiki
* various specialized and/or local stats keeper projects, for example [road km's in Italy](http://www.gfoss.it/osm/stat/), [Wochenaufgabe](http://blog.openstreetmap.de/blog/category/wochenaufgabe/)'s [OSM Count](http://thefive.sabic.uberspace.de/table/GuidePost_Node.html) (currently offline), [
housenumber evaluation on regio-osm.de](http://regio-osm.de/hausnummerauswertung/theoretischeabdeckung.html), [Building coverage in Austria](https://osm-austria-building-coverage.thomaskonrad.at/)
