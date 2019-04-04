/*
	beatfreq - FFT-based Beat frequency calculator

    Copyright (C) 2003-2010  Nigel D. Stepp

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

	This program is DUAL LICENSED under the MIT License, which means you may
	choose the license terms appropriate for your use. See COPYING-GPL and
	COPYING-MIT for details.

    Nigel Stepp <stepp@atistar.net>

    $Id: list.c 613 2010-08-25 17:40:47Z stepp $
*/

#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "list.h"

/*
 * Simple function to add a node on the end of a linked list
 */
maxima_list_t *add_maximum(maxima_list_t *list,int index, data_t intensity)
{
	maxima_list_t *new_max,*p=list;

	new_max = malloc(sizeof(maxima_list_t));

	new_max->index     = index;
	new_max->intensity = intensity;
	new_max->next      = NULL;

	if(!p) {
		return(new_max);
	} else {
		while(p->next) {
			p = p->next;
		}

		p->next = new_max;
		return(list);
	}

	return(NULL);
}

/*
 * This adds a node to a linked list such that it maintains ordering
 */
maxima_list_t *add_maximum_sort(maxima_list_t *list,int index, data_t intensity)
{
	maxima_list_t *new_max,*p=list;

	new_max = malloc(sizeof(maxima_list_t));

	new_max->index     = index;
	new_max->intensity = intensity;
	new_max->next      = NULL;

	if(!list) {
		return(new_max);
	} else {
		if(new_max->intensity > p->intensity) {
			new_max->next = p;
			return(new_max);
		} else {
			while(p->next) {
				if(new_max->intensity > p->next->intensity) {
					new_max->next = p->next;
					p->next = new_max;
					return(list);
				}
				p=p->next;
			}
			p->next=new_max;
			return(list);
		}
	}

	return(NULL);
}

/*
 * Free a generic list
 */
void free_list(generic_list_t *list)
{
	generic_list_t *p=list;
	generic_list_t *pnext;

	while (p) {
		pnext = p->next;
		free(p);
		p = pnext;
	}
}

