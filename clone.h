//
// Created by ice10 on 2019/2/13.
// written by dear Natsu chan
//

#ifndef ARTEMIS_CLONE_H
#define ARTEMIS_CLONE_H

#include <memory>

using std::shared_ptr;
using std::make_shared;

template<typename T>
struct Clone {
	virtual ~Clone() = default;
	virtual shared_ptr<T> clone() = 0;
	virtual shared_ptr<const T> clone() const = 0;
};

template<typename Self, typename Base>
class DeriveClone : public Base {
	shared_ptr <Base> clone() override {
		return make_shared<Self>(*static_cast<Self *>(this));
	}

	shared_ptr<const Base> clone() const override {
		return make_shared<const Self>(*static_cast<const Self *>(this));
	}
};


#endif //ARTEMIS_CLONE_H
