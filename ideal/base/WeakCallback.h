
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : WeakCallback.h
*   Last Modified : 2019-07-09 15:34
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_WEAKCALLBACK_H
#define  _IDEAL_BASE_WEAKCALLBACK_H

#include <memory>
#include <functional>

namespace ideal {

template <typename CLASS, typename... ARGS>
class WeakCallback {
public:
    WeakCallback(const std::weak_ptr<CLASS>& object, const std::function<void(CLASS*, ARGS...)>& function) :
        _object(object),
        _function(function) {
    }

    void operator()(ARGS&&... args) const {
        std::shared_ptr<CLASS> ptr(_object.lock());
        if(ptr) {
            _function(ptr.get(), std::forward<ARGS>(args)...);
        }
    }


private:
    std::weak_ptr<CLASS> _object;
    std::function<void(CLASS*, ARGS...)> _function;
};

template <typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void(CLASS::*function)(ARGS...)) {
    return WeakCallback<CLASS, ARGS...>(object, function);
}

template <typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void(CLASS::*function)(ARGS...) const) {
    return WeakCallback<CLASS, ARGS...>(object, function);
}

}

#endif // _IDEAL_BASE_WEAKCALLBACK_H

