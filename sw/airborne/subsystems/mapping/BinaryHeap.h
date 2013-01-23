#ifndef BINARYHEAP_H_
#define BINARYHEAP_H_

template <class T1> struct Node {
	T1 data;
	int score;
};

#include <vector>

template <class T> class BinaryHeap {
	std::vector<Node<T *> > array;

public:
	/* bubbles the item at pos to the appropriate hight in the heap */
	void bubble(int pos) {
		int nextPos = (pos - 1)/2;
		Node<T *> current = array.at(pos);
		Node<T *> parent = array.at(nextPos);
		if(pos != 0 && current.score < parent.score) {
			array.at(pos) = parent;
			array.at(nextPos) = current;
			bubble(nextPos);
		}
	}

	/* Adds the item at the appropriate place (with minimum score at the top)*/
	void add(T * item, int score) {
		Node<T *> temp;
		temp.data = item;
		temp.score = score;

		array.push_back(temp);
		int pos = array.size() - 1;
		bubble(pos);
	}

	/* returns the top Node */
	T * get() {
		return array.front().data;
	}

	/* removes the top Node */
	void remove() {
		unsigned int pos = 0;
		Node<T *> last = array.back();
		array.pop_back();
		while((pos + 1)*2 - 1 < array.size()) {
			Node<T *> min = array.at((pos + 1)*2 - 1);
			int minPos = (pos + 1)*2 - 1;
			if ((pos + 1)*2 < array.size()) {
				Node<T *> otherChild = array.at((pos + 1)*2);
				if(otherChild.score < min.score) {
					min = otherChild;
					minPos = (pos + 1)*2;
				}
			}
			if(min.score < last.score) {
				array.at(pos) = min;
				pos = minPos;
			} else break;
		}
		if(!array.empty())
			array.at(pos) = last;
	}

	/* returns whether the heap is empty */
	bool empty() {
		return array.empty();
	}

	/* resorts the selected item (should be called when item value changes) */
	void resort(T * item, int score) {
		if(array.size() > 0) {
	  		int i = -1;
	  		Node<T *> current;
			do {
				i++;
				current = array.at(i);
			} while ((unsigned) i + 1 < array.size() && item != current.data);

			if((unsigned) i < array.size()) {
				array.at(i).score = score;
				bubble(i);
			}
		}
	}
};


#endif /* BINARYHEAP_H_ */

