/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef STREAMING_EXTRACTOR_H
#define STREAMING_EXTRACTOR_H

#include <essentia/streaming/sourcebase.h>
#include <essentia/pool.h>

void computeSegments(const std::string& audioFilename, essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options);
void compute(const std::string& audioFilename, const std::string& outputFilename,
             essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options);

void computeReplayGain(const std::string& audioFilename, essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options);
void computeLowLevel(const std::string& audioFilename, essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options,
                     essentia::Real startTime, essentia::Real endTime, const std::string& nspace = "");
void computeBeatTrack(essentia::Pool& pool, const essentia::Pool& options, const std::string& nspace = "");
void computeMidLevel(const std::string& audioFilename, essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options,
                     essentia::Real startTime, essentia::Real endTime, const std::string& nspace = "");
void computePanning(const std::string& audioFilename, essentia::Pool& neqloudPool, essentia::Pool& eqloudPool, const essentia::Pool& options,
                    essentia::Real startTime, essentia::Real endTime, const std::string& nspace = "");
void computeHighlevel(essentia::Pool& pool, const essentia::Pool& options, const std::string& nspace = "");
essentia::Pool computeAggregation(essentia::Pool& pool, const essentia::Pool& options, int segments=0);
void addSVMDescriptors(essentia::Pool& pool);
void outputToFile(essentia::Pool& pool, const std::string& outputFilename, const essentia::Pool& options);

int compute(const std::string& audioFilename, const std::string& outputFilename, const std::string& profileFilename = "");

#endif // STREAMING_EXTRACTOR_H
