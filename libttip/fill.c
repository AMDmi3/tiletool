/*
 * Copyright (C) 2012 Dmitry Marakasov
 *
 * This file is part of libttip.
 *
 * libttip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libttip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libttip.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include <ttip_int.h>

ttip_result_t ttip_clear(ttip_image_t target) {
	memset(target->data, 0, target->stride * target->height);

	return TTIP_OK;
}
