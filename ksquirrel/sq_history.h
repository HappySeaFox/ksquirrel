#ifndef  _SQUIRREL_HISTORY_H_
#define _SQUIRREL_HISTORY_H_

#include <qvaluestack.h>

template <class T>
class SQ_History : public QValueStack<T>
{
    public:
	  SQ_History() {}
	~SQ_History() {}

	void  push(const T &item)
	{
		if(this->count() > 0)
		{
        		T elem(this->last());

	         	if(elem == item)
		          	return;
		}

		QValueStack<T>::push(item);
	}
};

#endif
