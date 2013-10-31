/**
 * Yaafe
 *
 * Copyright (c) 2009-2010 Institut Télécom - Télécom Paristech
 * Télécom ParisTech / dept. TSI
 *
 * Author : Benoit Mathieu
 *
 * This file is part of Yaafe.
 *
 * Yaafe is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Yaafe is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ShapeStatistics.h"
#include "MathUtils.h"

using namespace std;
using namespace Eigen;

namespace YAAFE
{

ShapeStatistics::ShapeStatistics()
{
    m_outputCentroid = true;
    m_outputSpread = true;
    m_outputSkewness = true;
    m_outputKurtosis = true;
}

ShapeStatistics::~ShapeStatistics()
{
}

ParameterDescriptorList ShapeStatistics::getParameterDescriptorList() const
{
    ParameterDescriptorList pList;
    ParameterDescriptor p;

    p.m_identifier = "output";
    p.m_description = "List of statistics to output (default order and value: centroid,spread,skewness,kurtosis)";
    p.m_defaultValue = "centroid,spread,skewness,kurtosis";
    pList.push_back(p);

    return pList;
}

bool ShapeStatistics::init(const ParameterMap& params, const Ports<StreamInfo>& inp)
{
	assert(inp.size()==1);
	const StreamInfo& in = inp[0].data;

    string str = getStringParam("output",params);
    if(str!=""){
        m_outputCentroid = (str.find("centroid") != string::npos);
        m_outputSpread = (str.find("spread") != string::npos);
        m_outputSkewness = (str.find("skewness") != string::npos);
        m_outputKurtosis = (str.find("kurtosis") != string::npos);
    }

    int ports = 0;
    if(m_outputCentroid) ports++;
    if(m_outputSpread) ports++;
    if(m_outputSkewness) ports++;
    if(m_outputKurtosis) ports++;

    outStreamInfo().add(StreamInfo(in,ports));
    return true;
}

bool ShapeStatistics::process(Ports<InputBuffer*>& inp, Ports<OutputBuffer*>& outp)
{
	assert(inp.size()==1);
	InputBuffer* in = inp[0].data;
	if (in->empty()) return false;
	assert(outp.size()==1);
	OutputBuffer* out = outp[0].data;
	const int N = in->info().size;

    while (!in->empty())
    {
        double* input = in->readToken();

        // compute moments
    	double moments[4] = { 0.0,0.0,0.0,0.0 };
    	{
			double dataSum = 0;
			for (int i=0;i<N;i++)
			{
				double v = abs(input[i]);
				dataSum += v;
				v *= i;
				moments[0] += v;
				v *= i;
				moments[1] += v;
				v *= i;
				moments[2] += v;
				v *= i;
				moments[3] += v;
			}
			if (dataSum==0)
				dataSum = EPS;
			moments[0] /= dataSum;
			moments[1] /= dataSum;
			moments[2] /= dataSum;
			moments[3] /= dataSum;
    	}

        int o=0;

        double* output = out->writeToken();
    	// centroid

        double centroid = moments[0];
        if(m_outputCentroid)
            output[o++] = centroid;
        // spread
        double spread = sqrt(moments[1] - pow2(moments[0]));
        if (spread == 0)
            spread = EPS;
        if(m_outputSpread)
            output[o++] = spread;
        // skewness
        double skewness = (2 * pow3(moments[0]) - 3 * moments[0]
                * moments[1] + moments[2]) / pow3(spread);
        if(m_outputSkewness)
            output[o++] = skewness;
        // kurtosis
        double kurtosis = (-3 * pow4(moments[0]) + 6 * moments[0]
                * moments[1] - 4 * moments[0] * moments[2] + moments[3])
                / pow4(spread) - 3;
        if(m_outputKurtosis)
            output[o++] = kurtosis;

        in->consumeToken();
    }
    return true;
}

}

