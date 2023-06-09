/*
 *  osgPlugins.h
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 26/10/10
 *
 *  @copyright (c) 2010 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 *  Adapted from Thomas Hogarth's 2009 examples from:
 *  http://github.com/stmh/osg/tree/iphone
 */

#pragma once 

#include <osgViewer/GraphicsWindow>
#include <osgDB/Registry>


/* #undef _OPENTHREADS_ATOMIC_USE_GCC_BUILTINS */
/* #undef _OPENTHREADS_ATOMIC_USE_MIPOSPRO_BUILTINS */
/* #undef _OPENTHREADS_ATOMIC_USE_SUN */
/* #undef _OPENTHREADS_ATOMIC_USE_WIN32_INTERLOCKED */
/* #undef _OPENTHREADS_ATOMIC_USE_MUTEX */

#define OSG_LIBRARY_STATIC


//windowing system
USE_GRAPICSWINDOW_IMPLEMENTATION(IOS)

//plugins
USE_DOTOSGWRAPPER(AlphaFunc)
USE_DOTOSGWRAPPER(AnimationPath)
USE_DOTOSGWRAPPER(AutoTransform)
USE_DOTOSGWRAPPER(Billboard)
USE_DOTOSGWRAPPER(BlendColor)
USE_DOTOSGWRAPPER(BlendEquation)
USE_DOTOSGWRAPPER(BlendFunc)
USE_DOTOSGWRAPPER(Camera)
USE_DOTOSGWRAPPER(CameraView)
USE_DOTOSGWRAPPER(ClearNode)
USE_DOTOSGWRAPPER(ClipNode)
USE_DOTOSGWRAPPER(ClipPlane)
USE_DOTOSGWRAPPER(ClusterCullingCallback)
USE_DOTOSGWRAPPER(ColorMask)
USE_DOTOSGWRAPPER(ColorMatrix)
USE_DOTOSGWRAPPER(ConvexPlanarOccluder)
USE_DOTOSGWRAPPER(CoordinateSystemNode)
USE_DOTOSGWRAPPER(CullFace)
USE_DOTOSGWRAPPER(Depth)
USE_DOTOSGWRAPPER(Drawable)
USE_DOTOSGWRAPPER(EllipsoidModel)
USE_DOTOSGWRAPPER(Fog)
USE_DOTOSGWRAPPER(FragmentProgram)
USE_DOTOSGWRAPPER(FrontFace)
USE_DOTOSGWRAPPER(Geode)
USE_DOTOSGWRAPPER(Geometry)
USE_DOTOSGWRAPPER(Group)
USE_DOTOSGWRAPPER(Image)
USE_DOTOSGWRAPPER(ImageSequence)
USE_DOTOSGWRAPPER(Light)
USE_DOTOSGWRAPPER(LightModel)
USE_DOTOSGWRAPPER(LightSource)
USE_DOTOSGWRAPPER(LineStipple)
USE_DOTOSGWRAPPER(LineWidth)
USE_DOTOSGWRAPPER(LOD)
USE_DOTOSGWRAPPER(Material)
USE_DOTOSGWRAPPER(MatrixTransform)
USE_DOTOSGWRAPPER(NodeCallback)
USE_DOTOSGWRAPPER(Node)
USE_DOTOSGWRAPPER(Object)
USE_DOTOSGWRAPPER(OccluderNode)
USE_DOTOSGWRAPPER(OcclusionQueryNode)
USE_DOTOSGWRAPPER(PagedLOD)
USE_DOTOSGWRAPPER(Point)
USE_DOTOSGWRAPPER(PointSprite)
USE_DOTOSGWRAPPER(PolygonMode)
USE_DOTOSGWRAPPER(PolygonOffset)
USE_DOTOSGWRAPPER(PositionAttitudeTransform)
USE_DOTOSGWRAPPER(Program)
USE_DOTOSGWRAPPER(Projection)
USE_DOTOSGWRAPPER(ProxyNode)
USE_DOTOSGWRAPPER(Scissor)
USE_DOTOSGWRAPPER(Sequence)
USE_DOTOSGWRAPPER(ShadeModel)
USE_DOTOSGWRAPPER(Shader)
USE_DOTOSGWRAPPER(Sphere)
USE_DOTOSGWRAPPER(Cone)
USE_DOTOSGWRAPPER(Capsule)
USE_DOTOSGWRAPPER(Box)
USE_DOTOSGWRAPPER(HeightField)
USE_DOTOSGWRAPPER(CompositeShape)
USE_DOTOSGWRAPPER(Cylinder)
USE_DOTOSGWRAPPER(ShapeDrawable)
USE_DOTOSGWRAPPER(StateAttribute)
USE_DOTOSGWRAPPER(StateSet)
USE_DOTOSGWRAPPER(Stencil)
USE_DOTOSGWRAPPER(Switch)
USE_DOTOSGWRAPPER(TessellationHints)
USE_DOTOSGWRAPPER(TexEnvCombine)
USE_DOTOSGWRAPPER(TexEnv)
USE_DOTOSGWRAPPER(TexEnvFilter)
USE_DOTOSGWRAPPER(TexGen)
USE_DOTOSGWRAPPER(TexGenNode)
USE_DOTOSGWRAPPER(TexMat)
USE_DOTOSGWRAPPER(Texture1D)
USE_DOTOSGWRAPPER(Texture2D)
USE_DOTOSGWRAPPER(Texture3D)
USE_DOTOSGWRAPPER(Texture)
USE_DOTOSGWRAPPER(TextureCubeMap)
USE_DOTOSGWRAPPER(TextureRectangle)
USE_DOTOSGWRAPPER(Transform)
USE_DOTOSGWRAPPER(Uniform)
USE_DOTOSGWRAPPER(VertexProgram)
USE_DOTOSGWRAPPER(Viewport)

//USE_OSGPLUGIN(obj)
//USE_OSGPLUGIN(ive)
USE_OSGPLUGIN(osg)

USE_OSGPLUGIN(imageio)

//USE_OSGPLUGIN(freetype)
