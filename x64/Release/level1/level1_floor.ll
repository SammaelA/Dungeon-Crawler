map {
  _filename = "level1/level1_floor.png"
}

texture {
    _name = "floor1_tex"
    _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_829.png"
}

tile {
    _name = "floor_tile"
    _texture = "floor1_tex"
}

entity {
  _type = "Floor"
  _tile = "floor_tile"
  _patterned = true
  _colorBinding = #FFFFFF
}
