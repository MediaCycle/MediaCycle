/**
 * Yaafe
 *
 * Copyright (c) 2009-2010 Institut TÃ©lÃ©com - TÃ©lÃ©com Paristech
 * TÃ©lÃ©com ParisTech / dept. TSI
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

#include "DataFlowParserContext.h"

namespace YAAFE {

DataFlowContext::DataFlowContext() :
	m_dataflow(NULL), m_identifiers(), m_params()
{}

DataFlowContext::~DataFlowContext()
{}

}
