#pragma once

struct Coordinates {
  double lat;
  double lon;

  Coordinates() : Coordinates(0, 0) {}

  Coordinates(double lat, double lon) : lat(lat), lon(lon) {}

  bool operator==(const Coordinates &other) const {
    return this->lat == other.lat && this->lon == other.lon;
  }
};








double distBetween2Points(Coordinates p1, Coordinates p2);



Coordinates centerBetween2Points(Coordinates p1, Coordinates p2);
