/*****************************************************************************/
// File         : connectivity.hpp
// Author       : Zhihua Ban
// Cotact       : sawpara@126.com
// Last Revised : 2017-1-19
/*****************************************************************************/
// Copyright 2017 Zhihua Ban. All rights reserved.
/*****************************************************************************/
// Desc : Enforce connectivity of superpixels
/*****************************************************************************/
#ifndef __CONNECTIVITY_HPP__
#define __CONNECTIVITY_HPP__


#include <algorithm>
#include <vector>
#include <cfloat>
using namespace std;

namespace sp{
	struct MergeCell{
		int x, y, n;
	};
	template<class T>
	bool mcless(const T &a, const T &b){
		return a.n < b.n;
	}
	struct Dist{
		int l; // neighbor label
		float d;
	};

	template <class T>
	class Merge{
	public:

		int merge(T *ilabels, int lsteps, PixI *color, int csteps, int width, int height, int min_size){
      // find all connected segments
			_relabel(ilabels, lsteps, width, height);
      // sort them by size, and 
			return _merge(ilabels, lsteps, color, csteps, width, height, min_size);
		}

		void _relabel(T *ilabels, int lsteps, int width, int height){
			const int dx8[] = { +1, -1, 0, 0, -1, +1, +1, -1 };
			const int dy8[] = { 0, 0, +1, -1, -1, -1, +1, +1 };

			MergeCell mc;

			mlabels.resize(width*height);
			mx.resize(width*height);
			my.resize(width*height);
			mcount.resize(0);

			for (int i = 0; i < height*width; i++){
				mlabels[i] = -1;
			}
			int old_lab = 0;
			int new_lab = 0;
			int index;
			for (int y = 0; y < height; y++){
				for (int x = 0; x < width; x++){

					// if has new label, then continue.
					if (mlabels[y*width + x]>-1) continue;

					mlabels[y*width + x] = new_lab;
					old_lab = ilabels[y*lsteps + x];
					mx[0] = x;
					my[0] = y;

					// if has no new label, count the number 
					int count = 1;
					for (int c = 0; c < count; c++){
						for (int d = 0; d < 8; d++){
							int xx = mx[c] + dx8[d];
							int yy = my[c] + dy8[d];
							// boundary check
							if (xx < 0 || xx >= width || yy < 0 || yy >= height) continue;
							if (mlabels[yy*width + xx] < 0 && ilabels[yy*lsteps + xx] == old_lab){
								mx[count] = xx;
								my[count] = yy;
								mlabels[yy*width + xx] = new_lab;
								count++;
							}
						}
					}
					mc.x = mx[0];	mc.y = my[0];	mc.n = count;
					mcount.push_back(mc);
					new_lab++;
				}
			}
			for (int y = 0; y < height; y++){
				for (int x = 0; x < width; x++){
					ilabels[y*lsteps + x] = mlabels[y*width + x];
				}
			}
		}

		int _merge(T* ilabels, int lsteps, PixI *color, int csteps, int width, int height, int min_size){
			const int dx8[] = { +1, -1, 0, 0, -1, +1, +1, -1 };
			const int dy8[] = { 0, 0, +1, -1, -1, -1, +1, +1 };
			mdist.resize(width*height);
			std::sort(mcount.begin(), mcount.end(), mcless<MergeCell>);
			mlookup_big2small.resize(mcount.size());
			mlookup_small2big.resize(mcount.size());
			mcount_extra.resize(mcount.size());
			memset(&mcount_extra[0], 0, mcount_extra.size()*sizeof(mcount_extra[0]));
			for (int i = 0; i < mlookup_big2small.size(); i++){
				mlookup_big2small[i] = -1;
				mlookup_small2big[i] = -1;
			}

			vector<double> meandist;
			vector<int> meandist_count;

			for (int i = 0; i < width*height; i++){
				mlabels[i] = 0; // not accessed
			}

			int count, nei_count;
			int old_label, new_label;

			int sp_count = mcount.size();

			for (int i = 0; i < mcount.size(); i++){
				mx[0] = mcount[i].x; my[0] = mcount[i].y;
				old_label = ilabels[my[0] * lsteps + mx[0]];

				if (mcount[i].n + mcount_extra[old_label] < min_size){
					sp_count--;
					// find its neast neighbour
					meandist.clear();
					meandist_count.clear();
					count = 1;	nei_count = 0;
					mlabels[my[0] * width + mx[0]] = 1; // accessed
					for (int c = 0; c < count; c++){
						PixI &mep = color[my[c] * csteps + mx[c]];

						for (int d = 0; d < 8; d++){
							int xx = mx[c] + dx8[d];
							int yy = my[c] + dy8[d];
							// boundary check
							if (xx < 0 || xx >= width || yy < 0 || yy >= height) continue;

							if (mlabels[yy*width + xx] == 0){ // if (xx,yy) have not been accessed
								new_label = ilabels[yy*lsteps + xx];
								if (new_label == old_label){ // if the label of (xx,yy) is identcal to the label of (mx[0], my[0])
									mx[count] = xx;
									my[count] = yy;
									count++;
									mlabels[yy*width + xx] = 1; // accessed
								}
								else{ // if (xx,yy) is a pixel from (mx[c],my[c])'s neighboring superpixel
									PixI &oth = color[yy*csteps + xx];
									mdist[nei_count].l = new_label;
									mdist[nei_count].d = abs(mep.f0() - oth.f0()) + abs(mep.f1() - oth.f1()) + abs(mep.f2() - oth.f2());
									nei_count++;
									if (mlookup_big2small[new_label] == -1){
										mlookup_big2small[new_label] = meandist.size();
										mlookup_small2big[meandist.size()] = new_label;
										meandist.push_back(0);
										meandist_count.push_back(0);
									}
								}
							}
						} // end searching the neighbouring pixels of (mx[c],my[c])
					}// end accessing superpixel of old_label

					for (int k = 0; k < nei_count; k++){
						meandist[mlookup_big2small[mdist[k].l]] += mdist[k].d;
						meandist_count[mlookup_big2small[mdist[k].l]]++;
					}
					new_label = -1;
					double min_dist = DBL_MAX;
					for (int k = 0; k < meandist.size(); k++){
						meandist[k] /= double(meandist_count[k]);
						if (min_dist>meandist[k]){
							min_dist = meandist[k];
							new_label = mlookup_small2big[k];
						}
					}
					for (int k = 0; k < meandist.size(); k++){
						mlookup_big2small[mlookup_small2big[k]] = -1;
					}

					// relabel itself
					for (int c = 0; c < count; c++){
						ilabels[my[c] * lsteps + mx[c]] = new_label;
						mlabels[my[c] * width + mx[c]] = 0; // not have been accessed
					}
					mcount_extra[new_label] += mcount[i].n;

				}
			}

			return sp_count;
		}
	protected:


	private:
		vector<T> mx;
		vector<T> my;
		vector<T> mlabels;
		vector<MergeCell> mcount;
		vector<int> mcount_extra;
		vector<Dist> mdist;
		vector<int> mlookup_big2small;
		vector<int> mlookup_small2big;
	};

}

#endif
