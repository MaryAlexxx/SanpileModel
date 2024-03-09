#include "StringValues.h"

string_values::string_values(const std::string& pc, const std::string& ps) {
    full_argument_ = ps;
    short_argument_ = pc;

    value_ptr_ = nullptr;
    value_str_ptr_ = nullptr;

    min_args_count_ = 0;
    multi_value_full_ = true;

    one_value_ = true;
    positional_ = false;
    store_value_ = false;
    default_ = false;
    used_ = false;

}

string_values::string_values(char& pc, const std::string& ps, const std::string& description) {
    full_argument_ = ps;
    short_argument_ = pc;
    this->description_ = description;

    value_ptr_ = nullptr;
    value_str_ptr_ = nullptr;

    min_args_count_ = 0;
    multi_value_full_ = true;

    one_value_ = true;
    positional_ = false;
    store_value_ = false;
    default_ = false;
    used_ = false;
}

bool string_values::isPositional() const {
    return positional_;
}

bool string_values::isUsed() const {
    return used_;
}

bool string_values::isMultiValueFull() const {
    return multi_value_full_;
}

std::pair<std::string, std::string> string_values::get_arguments() const {
    return {short_argument_, full_argument_};
}

std::string string_values::argumentName() const {
    return full_argument_.empty() ? short_argument_ : full_argument_;
}

std::string string_values::get_value(size_t ind) const {
    if (!used_) {
        throw std::invalid_argument("There's no value in parameter : " + argumentName());
    }

    if (store_value_ && ((one_value_ && value_str_ptr_ == nullptr) || (!one_value_ && value_ptr_ == nullptr))) {
        throw std::invalid_argument("There's no pointer to the variable in the argument : " + argumentName());
    }

    if (!one_value_) {
        if (ind == -1) {
            throw std::invalid_argument("There's no index was given");
        }

        if (store_value_ && ind >= value_ptr_->size()) {
            throw std::out_of_range("Index is out of range.");
        }

        if (!store_value_ && ind >= value_.size()) {
            throw std::out_of_range("Index is out of range.");
        }
    }


    if (one_value_) {
        return store_value_ ? *value_str_ptr_ : value_str_;
    }
    return store_value_ ? (*value_ptr_)[ind] : value_[ind];

}

void string_values::add(const std::string& x) {
    if (used_ && !default_ && one_value_) {
        throw std::invalid_argument("Argument : " + argumentName() + ", has already had a value");
    }

    if (store_value_ && ((one_value_ && value_str_ptr_ == nullptr) || (!one_value_ && value_ptr_ == nullptr))) {
        throw std::invalid_argument("There's no initialized variable for StoreValue.");
    }

    if (one_value_) {
        if (store_value_) {
            *value_str_ptr_ = x;
            used_ = true;
        } else {
            value_str_ = x;
            used_ = true;
        }
    } else {
        if (store_value_) {
            value_ptr_->push_back(x);
            multi_value_full_ = value_ptr_->size() >= min_args_count_;
            used_ = true;
        } else {
            value_.push_back(x);
            multi_value_full_ = value_.size() >= min_args_count_;
            used_ = true;
        }
    }

}

string_values& string_values::MultiValue(size_t i) {
    if (default_) {
        throw std::logic_error("Argument " + argumentName() + " has a default value. It can't be multi-value");
    }

    min_args_count_ = i;
    one_value_ = false;
    return *this;
}

string_values& string_values::MultiValue() {
    if (default_) {
        throw std::logic_error("Argument " + argumentName() + " has a default value. It can't be multi-value");
    }

    one_value_ = false;
    return *this;
}

string_values& string_values::StoreValue(std::string& variable) {
    if (!one_value_) {
        throw std::logic_error(argumentName() + " is multi-value argument");
    }

    store_value_ = true;
    value_str_ptr_ = &variable;

    return *this;
}

string_values& string_values::StoreValues(std::vector<std::string>& variable) {
    if (one_value_) {
        throw std::logic_error(argumentName() + " is one-value argument");
    }

    store_value_ = true;
    value_ptr_ = &variable;

    return *this;
}

string_values& string_values::Positional() {
    positional_ = true;
    return *this;
}

string_values& string_values::Default(const std::string& value) {
    if (!one_value_) {
        throw std::logic_error(".Default is only for one-value arguments");
    }

    if (store_value_ && value_str_ptr_ == nullptr) {
        throw std::invalid_argument("There's no pointer to the variable in the argument : " + argumentName());
    }

    if (store_value_) {
        *value_str_ptr_ = value;
        default_ = true;
        used_ = true;
    } else {
        value_str_ = value;
        default_ = true;
        used_ = true;
    }

    return *this;
}
