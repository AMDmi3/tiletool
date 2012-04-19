/*
 * Copyright (C) 2012 Dmitry Marakasov
 *
 * This file is part of tiletool.
 *
 * tiletool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tiletool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tiletool.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EMPTYTILE_H
#define EMPTYTILE_H

void init_empty_tile(const char* path);
void cleanup_empty_tile();

int has_empty_tile();
ttip_image_t spawn_empty_tile();

#endif
