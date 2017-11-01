// sala - a component of the depthmapX - spatial network analysis platform
// Copyright (C) 2011-2012, Tasos Varoudis

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <math.h>
#include <float.h>
#include <time.h>
#include <genlib/paftl.h>

#include <salalib/mgraph.h> // purely for the version info --- as phased out should replace

#include "isovist.h"

///////////////////////////////////////////////////////////////////////

// Interestingly, apparently ray tracing is faster using voxel techniques than octrees etc:
// Akira Fujimotot, Takayuki Tanaka, and Kansei Iwata. ARTS: Accelerated ray-tracing system.  IEEE Computer Graphics and Applications, 6(4):16--26, April 1986
 
// This uses BSP trees, and appears to be superfast once the tree is built

void Isovist::makeit(BSPNode *root, const Point2f& p, const QtRegion& region, double startangle, double endangle)
{
   // region is used to give an idea of scale, so isovists can be linked when there is floating point error
   double tolerance = __max(region.width(),region.height()) * 1e-9;

   m_centre = p;
   m_blocks.clear();
   m_gaps.clear();

   // still doesn't work when need centre point, but this will work for 180 degree isovists
   bool complete = false;

   if (startangle == endangle || (startangle == 0.0 && endangle == 2.0 * M_PI)) {
      startangle = 0.0;
      endangle = 2.0 * M_PI;
      complete = true;
   }

   bool parity = false;

   if (startangle > endangle) {
      m_gaps.push_back(IsoSeg(0.0,endangle));
      m_gaps.push_back(IsoSeg(startangle,2.0*M_PI));
   }
   else {
      parity = true;
      m_gaps.push_back(IsoSeg(startangle,endangle));
   }

   make(root);

   // now it is constructed, make the isovist polygon:
   m_poly.clear();
   m_perimeter = 0.0;
   m_occluded_perimeter = 0.0;
   m_occlusion_points.clear();

   bool markedcentre = false;
   for (size_t i = 0; i < m_blocks.size(); i++) {
      if (!complete && !markedcentre && !parity && m_blocks[i].startangle == startangle) {
         // centre
         m_poly.push_back(p);
         // perimeter! occlusivity!
         markedcentre = true;
      }
      if (i != 0 && !approxeq(m_blocks[i-1].endpoint, m_blocks[i].startpoint, tolerance)) {
         m_poly.push_back(m_blocks[i].startpoint);
         // record perimeter information:
         double occluded = dist(m_blocks[i-1].endpoint,m_blocks[i].startpoint);
         m_perimeter += occluded;
         m_occluded_perimeter += occluded;
         // record the near *point* for use in agent analysis
         // (as the point will not move between isovists, so can record *which* occlusion this is, and spot novel ones)
         if (dist(m_blocks[i-1].endpoint,m_centre) < dist(m_blocks[i].startpoint,m_centre)) {
            m_occlusion_points.push_back(PointDist(m_blocks[i-1].endpoint,occluded));
         }
         else {
            m_occlusion_points.push_back(PointDist(m_blocks[i].startpoint,occluded));
         }
      }
      m_poly.push_back(m_blocks[i].endpoint);
      m_perimeter += dist(m_blocks[i].startpoint,m_blocks[i].endpoint);
   }
   // for some reason to do with ordering, if parity is true, the centre point must be last not first
   if (!complete && parity) {
      // centre
      m_poly.push_back(p);
      // perimeter! occlusivity!
   }
   if (m_blocks.size() && !approxeq(m_blocks.tail().endpoint, m_blocks.head().startpoint, tolerance)) {
      m_poly.push_back(m_blocks.head().startpoint);
      // record perimeter information:
      double occluded = dist(m_blocks.tail().endpoint,m_blocks.head().startpoint);
      m_perimeter += occluded;
      m_occluded_perimeter += occluded;
      // record the near *point* for use in agent analysis
      // (as the point will not move between isovists, so can record *which* occlusion this is, and spot novel ones)
      if (occluded > 1.5) {
         if (dist(m_blocks.tail().endpoint,m_centre) < dist(m_blocks.head().startpoint,m_centre)) {
            m_occlusion_points.push_back(PointDist(m_blocks.tail().endpoint,occluded));
         }
         else {
            m_occlusion_points.push_back(PointDist(m_blocks.head().startpoint,occluded));
         }
      }
   }
}

int Isovist::getClosestLine(BSPNode *root, const Point2f& p)
{
   m_centre = p;
   m_blocks.clear();
   m_gaps.clear();

   m_gaps.push_back(IsoSeg(0.0,2.0*M_PI));

   make(root);

   int mintag = -1;
   double mindist = 0.0;

   for (size_t i = 0; i < m_blocks.size(); i++) {
      Line l(m_blocks[i].startpoint, m_blocks[i].endpoint);
      if (mintag == -1 || dist(p,l) < mindist) {
         mindist = dist(p,l);
         mintag = m_blocks[i].tag;
      }
   }

   return mintag;
}

void Isovist::make(BSPNode *here)
{
   if (m_gaps.size()) {
      int which = here->classify(m_centre);
      if (which == BSPNode::BSPLEFT) {
         if (here->left)
            make(here->left);
         drawnode(here->line,here->m_tag);
         if (here->right)
            make(here->right);
      }
      else {
         if (here->right)
            make(here->right);
         drawnode(here->line,here->m_tag);
         if (here->left)
            make(here->left);
      }
   }
}

void Isovist::drawnode(const Line& li, int tag)
{
   Point2f p1 = li.start() - m_centre;
   p1.normalise();
   double angle1 = p1.angle();
   Point2f p2 = li.end() - m_centre;
   p2.normalise();
   double angle2 = p2.angle();
   if (angle2 > angle1) {
      if (angle2 - angle1 >= M_PI) {
         // 0 to angle1 and angle2 to 2 pi
         addBlock(li,tag,0.0,angle1);
         addBlock(li,tag,angle2,2.0*M_PI);
      }
      else {
         // angle1 to angle2
         addBlock(li,tag,angle1,angle2);
      }
   }
   else {
      if (angle1 - angle2 >= M_PI) {
         // 0 to angle2 and angle1 to 2 pi
         addBlock(li,tag,0.0,angle2);
         addBlock(li,tag,angle1,2.0*M_PI);
      }
      else {
         // angle2 to angle1
         addBlock(li,tag,angle2,angle1);
      }
   }
   // 
   for (size_t i = m_gaps.size() - 1; i != paftl::npos; i--) {
      if (m_gaps[i].tagdelete) {
         m_gaps.remove_at(i);
      }
   }
}

void Isovist::addBlock(const Line& li, int tag, double startangle, double endangle)
{
   int gap = 0;
   bool finished = false;

   while (!finished) {
      while (gap < (int)m_gaps.size() && m_gaps[gap].endangle < startangle) {
         gap++;
      }
      if (gap < (int)m_gaps.size() && m_gaps[gap].startangle < endangle + 1e-9) {
         double a,b;
         if (m_gaps[gap].startangle > startangle - 1e-9) {
            a = m_gaps[gap].startangle;
            if (m_gaps[gap].endangle < endangle + 1e-9) {
               b = m_gaps[gap].endangle;
               m_gaps[gap].tagdelete = true;
            }
            else {
               b = endangle;
               m_gaps[gap].startangle = endangle;
            }
         }
         else {
            a = startangle;
            if (m_gaps[gap].endangle < endangle + 1e-9) {
               b = m_gaps[gap].endangle;
               m_gaps[gap].endangle = startangle;
            }
            else {
               b = endangle;
               m_gaps.add(IsoSeg(endangle, m_gaps[gap].endangle, m_gaps[gap].quadrant));
               m_gaps[gap].endangle = startangle;
               gap++; // advance past gap just added
            }
         }
         Point2f pa = intersection_point(li,Line(m_centre,m_centre+pointfromangle(a)));
         Point2f pb = intersection_point(li,Line(m_centre,m_centre+pointfromangle(b)));
         m_blocks.add(IsoSeg(a,b,pa,pb,tag));
      }
      else {
         finished = true;
      }
      gap++;
   }
}

void addIsovistColumns(dXreimpl::AttributeTable<dXreimpl::SerialisedPixelRef> &table, bool simple_version)
{
    table.getOrInsertColumn("Isovist Area");
    if (!simple_version)
    {
        table.getOrInsertColumn("Isovist Compactness");
        table.getOrInsertColumn("Isovist Drift Angle");
        table.getOrInsertColumn("Isovist Drift Magnitude");
        table.getOrInsertColumn("Isovist Min Radial");
        table.getOrInsertColumn("Isovist Max Radial");
        table.getOrInsertColumn("Isovist Occlusivity");
        table.getOrInsertColumn("Isovist Perimeter");
    }

}

void Isovist::setData(dXreimpl::AttributeRow& row, bool simple_version)
{
   // the area / centre of gravity calculation is a duplicate of the SalaPolygon version,
   // included here for general information about the isovist
   double area = 0.0;
   Point2f centroid = Point2f(0,0);
   for (size_t i = 0; i < m_poly.size(); i++) {
      Point2f& p1 = m_poly.at(i);
      Point2f& p2 = m_poly.at((i+1)%m_poly.size());
      double a_i = (p1.x * p2.y - p2.x * p1.y) / 2.0;
      area += a_i ;
      a_i /= 6.0;
      centroid.x += (p1.x+p2.x) * a_i;
      centroid.y += (p1.y+p2.y) * a_i;
      double dpoint = dist(m_centre,p1);
      double dline = dist(m_centre,Line(p1,p2));
      if (i != 0) {
         // This is not minimum radial -- it's the distance to the closest corner!
         if (dline < m_min_radial) {
            m_min_radial = dline;
            dist(m_centre,Line(p1,p2));
         }
         if (dpoint > m_max_radial) {
            m_max_radial = dpoint;
         }
      }
      else {
         m_max_radial = dpoint;
         m_min_radial = dline;
      }
   }
   centroid.scale(2.0/fabs(area));

   Point2f driftvec = centroid - m_centre;
   double driftmag = driftvec.length();
   driftvec.normalise();
   double driftang = driftvec.angle();
   //
   row.setValue("Isovist Area",(float)area);

   // dX simple version test // TV
//#define _COMPILE_dX_SIMPLE_VERSION
#ifndef _COMPILE_dX_SIMPLE_VERSION
   if(!simple_version) {
       row.setValue("Isovist Compactness", (float)(4.0 * M_PI * area / (m_perimeter*m_perimeter)));
       row.setValue("Isovist Drift Angle",(float)(180.0*driftang/M_PI));
       row.setValue("Isovist Drift Magnitude",(float)driftmag);
       row.setValue("Isovist Min Radial",(float)m_min_radial);
       row.setValue("Isovist Max Radial",(float)m_max_radial);
       row.setValue("Isovist Occlusivity",(float)m_occluded_perimeter);
       row.setValue("Isovist Perimeter",(float)m_perimeter);
   }
#endif

}
