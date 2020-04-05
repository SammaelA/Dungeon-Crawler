map {
  _filename = "debugLevel.png"
}

texture {
  _name = "walls_tex"
  _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_59.png"
}

texture {
  _name = "player_tex"
  _filename = "tileset/Sliced/creatures_24x24/oryx_16bit_fantasy_creatures_01.png"
}

texture {
  _name = "enemy_tex"
  _filename = "tileset/Sliced/creatures_24x24/oryx_16bit_fantasy_creatures_04.png"
}

texture {
  _name = "pot_tex"
  _filename = "tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_20.png"
}

texture {
  _name = "sword_tex"
  _filename = "tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_200.png"
}

texture {
  _name = "potion_tex"
  _filename = "tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_01.png"
}

tile {
  _name = "item1_tile"
  _texture = "pot_tex"
}

tile {
  _name = "walls_tile"
  _texture = "walls_tex"
}

tile {
  _name = "player_tile"
  _texture = "player_tex"
}

tile {
  _name = "enemy_tile"
  _texture = "enemy_tex"
}

tile {
  _name = "sword_tile"
  _texture = "sword_tex"
}

tile {
  _name = "potion_tile"
  _texture = "potion_tex"
}

entity {
  _type = "Walls"
  _tile = "walls_tile"
  _patterned = true
  _colorBinding = #FFFFFF
}

entity {
  _type = "Collectable"
  _tile = "item1_tile"
  _colorBinding = #FFFF00
}