#include "val.h"

// Overloaded addition operator
Value Value::operator+(const Value& op) const {
    if (IsInt()) {
        if (op.IsInt()) {
            return Value(GetInt() + op.GetInt());
        } else if (op.IsReal()) {
            return Value(GetInt() + op.GetReal());
        } else if (op.IsChar()) {
            return Value(GetInt() + static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for addition");
        }
    }
    else if (IsReal()) {
        if (op.IsInt()) {
            return Value(GetReal() + op.GetInt());
        } else if (op.IsReal()) {
            return Value(GetReal() + op.GetReal());
        } else if (op.IsChar()) {
            return Value(GetReal() + static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for addition");
        }
    }
    else if (IsChar()) {
        int thisCharValue = static_cast<int>(GetChar());
        if (op.IsInt()) {
            return Value(thisCharValue + op.GetInt());
        } else if (op.IsReal()) {
            return Value(thisCharValue + op.GetReal());
        } else if (op.IsChar()) {
            return Value(thisCharValue + static_cast<int>(op.GetChar()));
        } else if (op.IsString()) {
            return Value(string(1, GetChar()) + op.GetString());
        } else {
            throw runtime_error("Invalid operands for addition");
        }
    }
    else if (IsString()) {
        if (op.IsString()) {
            return Value(GetString() + op.GetString());
        } else if (op.IsChar()) {
            return Value(GetString() + op.GetChar());
        } else {
            throw runtime_error("Invalid operands for addition");
        }
    }
    else {
        throw runtime_error("Invalid operands for addition");
    }
}



// Overloaded subtraction operator
Value Value::operator-(const Value& op) const {
    if (IsInt()) {
        if (op.IsInt()) {
            return Value(GetInt() - op.GetInt());
        } else if (op.IsReal()) {
            return Value(GetInt() - op.GetReal());
        } else if (op.IsChar()) {
            return Value(GetInt() - static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for subtraction");
        }
    } else if (IsReal()) {
        if (op.IsInt()) {
            return Value(GetReal() - op.GetInt());
        } else if (op.IsReal()) {
            return Value(GetReal() - op.GetReal());
        } else if (op.IsChar()) {
            return Value(GetReal() - static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for subtraction");
        }
    } else if (IsChar()) {
        int thisCharValue = static_cast<int>(GetChar());
        if (op.IsInt()) {
            return Value(thisCharValue - op.GetInt());
        } else if (op.IsReal()) {
            return Value(thisCharValue - op.GetReal());
        } else if (op.IsChar()) {
            return Value(thisCharValue - static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for subtraction");
        }
    } else {
        throw runtime_error("Invalid operands for subtraction");
    }
}



// Overloaded multiplication operator
Value Value::operator*(const Value& op) const {
    if (IsInt()) {
        if (op.IsInt()) {
            return Value(GetInt() * op.GetInt());
        } else if (op.IsReal()) {
            return Value(GetInt() * op.GetReal());
        } else if (op.IsChar()) {
            return Value(GetInt() * static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for multiplication");
        }
    } else if (IsReal()) {
        if (op.IsInt()) {
            return Value(GetReal() * op.GetInt());
        } else if (op.IsReal()) {
            return Value(GetReal() * op.GetReal());
        } else if (op.IsChar()) {
            return Value(GetReal() * static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for multiplication");
        }
    } else if (IsChar()) {
        int thisCharValue = static_cast<int>(GetChar());
        if (op.IsInt()) {
            return Value(thisCharValue * op.GetInt());
        } else if (op.IsReal()) {
            return Value(thisCharValue * op.GetReal());
        } else if (op.IsChar()) {
            return Value(thisCharValue * static_cast<int>(op.GetChar()));
        } else {
            throw runtime_error("Invalid operands for multiplication");
        }
    } else {
        throw runtime_error("Invalid operands for multiplication");
    }
}


// Overloaded division operator
Value Value::operator/(const Value& op) const {
    if (IsInt()) {
        if (op.IsInt()) {
            if (op.GetInt() == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(static_cast<double>(GetInt()) / op.GetInt());
        } else if (op.IsReal()) {
            if (op.GetReal() == 0.0) {
                throw runtime_error("Division by zero");
            }
            return Value(GetInt() / op.GetReal());
        } else if (op.IsChar()) {
            int opValue = static_cast<int>(op.GetChar());
            if (opValue == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(static_cast<double>(GetInt()) / opValue);
        } else {
            throw runtime_error("Invalid operands for division");
        }
    } else if (IsReal()) {
        if (op.IsInt()) {
            if (op.GetInt() == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(GetReal() / op.GetInt());
        } else if (op.IsReal()) {
            if (op.GetReal() == 0.0) {
                throw runtime_error("Division by zero");
            }
            return Value(GetReal() / op.GetReal());
        } else if (op.IsChar()) {
            int opValue = static_cast<int>(op.GetChar());
            if (opValue == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(GetReal() / opValue);
        } else {
            throw runtime_error("Invalid operands for division");
        }
    } else if (IsChar()) {
        int thisCharValue = static_cast<int>(GetChar());
        if (op.IsInt()) {
            if (op.GetInt() == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(static_cast<double>(thisCharValue) / op.GetInt());
        } else if (op.IsReal()) {
            if (op.GetReal() == 0.0) {
                throw runtime_error("Division by zero");
            }
            return Value(static_cast<double>(thisCharValue) / op.GetReal());
        } else if (op.IsChar()) {
            int opValue = static_cast<int>(op.GetChar());
            if (opValue == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(static_cast<double>(thisCharValue) / opValue);
        } else {
            throw runtime_error("Invalid operands for division");
        }
    } else {
        throw runtime_error("Invalid operands for division");
    }
}



// Overloaded remainder operator
Value Value::operator%(const Value& op) const {
    if (IsInt()) {
        if (op.IsInt()) {
            if (op.GetInt() == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(GetInt() % op.GetInt());
        } else if (op.IsChar()) {
            int opValue = static_cast<int>(op.GetChar());
            if (opValue == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(GetInt() % opValue);
        } else {
            throw runtime_error("Invalid operand for the REM operator"); // Updated message
        }
    } else if (IsChar()) {
        int thisCharValue = static_cast<int>(GetChar());
        if (op.IsInt()) {
            if (op.GetInt() == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(thisCharValue % op.GetInt());
        } else if (op.IsChar()) {
            int opValue = static_cast<int>(op.GetChar());
            if (opValue == 0) {
                throw runtime_error("Division by zero");
            }
            return Value(thisCharValue % opValue);
        } else {
            throw runtime_error("Invalid operand for the REM operator"); 
        }
    } else {
        throw runtime_error("Invalid operand for the REM operator");
    }
}



// Overloaded equality operator
Value Value::operator==(const Value& op) const {
    if (this->GetType() == op.GetType()) {
        if (this->IsInt()) {
            return Value(this->GetInt() == op.GetInt());
        } else if (this->IsReal()) {
            return Value(this->GetReal() == op.GetReal());
        } else if (this->IsBool()) {
            return Value(this->GetBool() == op.GetBool());
        } else if (this->IsChar()) {
            return Value(this->GetChar() == op.GetChar());
        } else if (this->IsString()) {
            return Value(this->GetString() == op.GetString());
        }
    } else {
        // Type promotion for numeric types
        if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal())) {
            double left = this->IsInt() ? this->GetInt() : this->GetReal();
            double right = op.IsInt() ? op.GetInt() : op.GetReal();
            return Value(left == right);
        } else {
            return Value(false); // Different types cannot be equal
        }
    }
    return Value(false);
}

// Overloaded inequality operator
Value Value::operator!=(const Value& op) const {
    return !(*this == op);
}

// Overloaded greater than operator
Value Value::operator>(const Value& op) const {
    if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal())) {
        double left = this->IsInt() ? this->GetInt() : this->GetReal();
        double right = op.IsInt() ? op.GetInt() : op.GetReal();
        return Value(left > right);
    } else {
        throw runtime_error("Runtime Error: Invalid operands for greater than comparison");
    }
}

// Overloaded less than operator
Value Value::operator<(const Value& op) const {
    if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal())) {
        double left = this->IsInt() ? this->GetInt() : this->GetReal();
        double right = op.IsInt() ? op.GetInt() : op.GetReal();
        return Value(left < right);
    } else {
        throw runtime_error("Runtime Error: Invalid operands for less than comparison");
    }
}

// Overloaded logical AND operator
Value Value::operator&&(const Value& op) const {
    if (this->IsBool() && op.IsBool()) {
        return Value(this->GetBool() && op.GetBool());
    } else {
        throw runtime_error("Runtime Error: Logical AND requires boolean operands");
    }
}

// Overloaded logical OR operator
Value Value::operator||(const Value& op) const {
    if (this->IsBool() && op.IsBool()) {
        return Value(this->GetBool() || op.GetBool());
    } else {
        throw runtime_error("Runtime Error: Logical OR requires boolean operands");
    }
}

// Overloaded logical NOT operator
Value Value::operator!(void) const {
    if (this->IsBool()) {
        return Value(!this->GetBool());
    } else {
        throw runtime_error("Runtime Error: Logical NOT requires a boolean operand");
    }
}
