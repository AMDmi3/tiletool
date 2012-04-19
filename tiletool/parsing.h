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

#ifndef PARSING_H
#define PARSING_H

int parse_double(const char* start, const char* end, double* out);
int parse_unsigned(const char* start, const char* end, int* out);
int parse_unsigned_range(const char* start, const char* end, int* out1, int* out2);
int parse_split(const char* start, char ch, const char** dividers, int parts);

#endif
