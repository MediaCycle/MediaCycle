/*
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 ************************************************
 * Also licensed with permission from Tom Tromey
 * and Owen Taylor under the Apache license.
 * Original location:
 * http://cvs.gnome.org/viewcvs/glib/glib/gutf8.c?rev=1.50&view=log
 ************************************************
 *
 * Copyright 2003-2006 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTF8_H
#define UTF8_H
#include <string>



size_t	mc_wctoutf8(char * outbuf, const wchar_t ch);
size_t mc_utf8towc(wchar_t& pwc, const char *p);
size_t mc_wcstoutf8(char * result, const wchar_t * str, size_t result_length);
size_t mc_utf8towcs(wchar_t * result, const char * str, size_t result_length);

#endif
