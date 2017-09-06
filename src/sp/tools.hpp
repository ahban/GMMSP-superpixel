/*****************************************************************************/
// File         : tools.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-8-29
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : tools for superpixel manipulation
/*****************************************************************************/

#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include <vector>
#include "su/matrix.hpp"
//#include "su/pixel.hpp"
//#include "sp/util.hpp"
namespace sp{
	class Util{
	public:
		
		template<class _ML>
		static int relabel(_ML &lab, const int ns){

			SU_ASSERT(lab.layers == 1, "matrix");
			int W = lab.width;
			int H = lab.height;
			// find min and max
			int lab_min = lab(0, 0);
			int lab_max = lab(0, 0);
			for (int y = 0; y < H; y++){
				for (int x = 0; x < W; x++){
					if (lab_max < lab(y, x))
						lab_max = lab(y, x);
					if (lab_min > lab(y, x))
						lab_min = lab(y, x);
				}
			}
			std::vector<int> m_map;
			// construct map
			int *p_map = NULL;
			if (lab_max < 0){
				lab_max = 0;
			}
			if (lab_min < 0){
				m_map.resize(lab_max + 1 - lab_min);
				p_map = &(m_map[-lab_min]);
			}
			else{
				m_map.resize(lab_max + 1);
				p_map = &(m_map[0]);
			}
			m_map.assign(m_map.size(), -1);

			int new_lab = ns;
			for (int y = 0; y < H; y++){
				for (int x = 0; x < W; x++){
					if (p_map[lab(y, x)] < 0){
						p_map[lab(y, x)] = new_lab;
						new_lab++;
					}
					lab(y, x) = p_map[lab(y, x)];
				}
			}

			return new_lab - ns;
		}
    
    
	};// end Utility class
  
} // end namespace 

#endif
