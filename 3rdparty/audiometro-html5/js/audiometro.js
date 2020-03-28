/**
 * @brief A script to render the AudioMetro layout using MediaCycle audio collections saved as JSON files.
 * @author Christian Frisson
 * @date 9/03/2015
 * @copyright (c) 2015 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

var w = 200,
	h = 200,
	x = d3.scale.linear().range([0, w]),
	y = d3.scale.linear().range([0, h]),
	node,
	nodes,
	tiles,
	resized = false,
	duration = 1000;

var i = 0;

var svg = d3.select("#body")
	.append("svg:svg")
	.attr("id", "graph")
	.attr("width", w)
	.attr("height", h)
	.append("svg:g")
	.attr("id", "box")

var treemap = d3.layout.treemap()
	.round(false)
	.size([w, h])
	.sticky(true)
	.value(function(d) {
		return d.size;
	});

function resize() {
	w = parseInt(d3.select("#graph").style("width")),
	h = parseInt(d3.select("#graph").style("height"));
	fh = parseInt(d3.select("#footer").style("height"));
	/*var nh = (w>h)?h:w;
	var nw = nh;
	w = nw
	h = nh*/
	h = h - fh

	d3.select("#graph")
		.attr("width", w)
		.attr("height", h)

	d3.select("#box")
		.attr("width", w)
		.attr("height", h)

	treemap.size([w, h]);

	resized = true;
	updateData();
}

d3.select(window).on('resize', resize);
resize();

function updateData() {

	d3.json("library.json", function(error, data) {

		layoutchangeduration = duration
		if (resized) {
			layoutchangeduration = 0
		}

		// Compute the new tree layout.
		nodes = treemap.nodes(data);
		tiles = treemap.nodes(data)
			.filter(function(d) {
				return !d.children && ('mcx' in d);
			});

		treemap.sticky(true); // necessary to reload the nodes  					

		var thumbnail = svg.selectAll("g")
		.data(tiles, function(d) {
			return d.name;
		});

		var entering = thumbnail.enter();

		var pos_scale = 0.9;
		var thumb_scale = 0.1;

		function updateColor(d) {
			var basecolor = d3.rgb("black").hsl();
			basecolor.l = 1 * d.perceptualSharpnessMean;
			return basecolor
		}

		function updateThumbnail() {
			thumbnail.select("svg").select("polygon")
				.attr("fill", function(d) {
					var basecolor = d3.rgb("black").hsl();
					basecolor.l = 1 * d.perceptualSharpnessMean;
					return basecolor
				})
				.attr("filename", function(d) {
					return d.name
				})

			thumbnail
			.transition()
				.duration(2 * layoutchangeduration)
				.attr("transform", function(d) {
					return "translate(" + (pos_scale * w * d.mcx) + "," + (pos_scale * h * d.mcy) + ")" + " scale(" + thumb_scale + ")";
				})
		};
		entering.append("g")
		.html(function(d) {
			var self = this;
			d3.xml(d.thumbnail, "image/svg+xml", function(xml) {
				var importedNode = document.importNode(xml.documentElement, true);
				self.appendChild(importedNode).cloneNode(true);
				var basecolor = d3.rgb("black").hsl()
				basecolor.l = 1 * d.perceptualSharpnessMean
				d3.select(self).select("polygon")
					.attr("fill", basecolor)
					.attr("filename", d.name)
			});
		})
			.attr("transform", function(d) {
				return "translate(" + (pos_scale * w * d.mcx) + "," + (pos_scale * h * d.mcy) + ")" + " scale(" + thumb_scale + ")";
			});

		entering.call(updateThumbnail);
		thumbnail.call(updateThumbnail);
		thumbnail.exit().remove();
		resized = false;
	});
}

updateData();
