#include <stdexcept>
#include <iostream>
#include <cassert>
#include "ast_context.hpp"

namespace ast {

    static int temporaries = 0;

    // We don't free temporaries automatically between functions - we assume there are no leaked register allocs
    // We didn't have time to implement a reliable register spilling/live range analysis approach so hopefully this is sufficient
    Register Context::AllocateTemporary(bool forFloat) {
        if (forFloat) {
            for (size_t i = 0; i < floatTemporaries_.size(); i++) {
                if (!floatTemporaries_.test(i)) {
                    ++temporaries;
                    floatTemporaries_.set(i);
                    return FloatTemporaryAtIndex(static_cast<int>(i));
                }
            }
        } else {
            for (size_t i = 0; i < integerTemporaries_.size(); i++) {
                if (!integerTemporaries_.test(i)) {
                    ++temporaries;
                    integerTemporaries_.set(i);
                    return IntegerTemporaryAtIndex(static_cast<int>(i));
                }
            }
        }
        // try allocating a persistent register
        return AllocatePersistent(forFloat);
    }

    void Context::FreeRegister(Register reg) {
        if (IsFloatRegister(reg)) {
            if (int index = IndexOfFloatTemporary(reg); index != -1) {
                if (!floatTemporaries_.test(index))
                    std::cerr << "Warning: freeing already free temporary" << std::endl;
                floatTemporaries_.reset(index);
            } else if (int pIndex = IndexOfFloatPersistent(reg); pIndex != -1) {
                if (!floatPersistent_.test(pIndex))
                    std::cerr << "Warning: freeing already free persistent" << std::endl;
                floatPersistent_.reset(pIndex);
            } else {
                std::cerr << "Warning: Attempted to free unexpected register" << std::endl;
            }
        } else {
            if (int index = IndexOfIntegerTemporary(reg); index != -1) {
                if (!integerTemporaries_.test(index))
                    std::cerr << "Warning: freeing already free temporary" << std::endl;
                integerTemporaries_.reset(index);
            } else if (int pIndex = IndexOfIntegerPersistent(reg); pIndex != -1) {
                if (!integerPersistent_.test(pIndex))
                    std::cerr << "Warning: freeing already free persistent" << std::endl;
                integerPersistent_.reset(pIndex);
            } else {
                std::cerr << "Warning: Attempted to free unexpected register" << std::endl;
            }
        }
        --temporaries;
    }

    Context::~Context() {
        if (temporaries != 0) {
            std::cerr << "Warning: " << temporaries << " temporary registers were not freed" << std::endl;
        }
    }

    Register Context::AllocatePersistent(bool forFloat) {
        ++temporaries; // must be in here now that we only have a single free method
        // Round robin allocation
        static size_t floatIndex = 0;
        static size_t intIndex = 1; // Avoid frame ptr

        if (forFloat) {
            for (size_t i = 0; i < floatPersistent_.size(); i++) {
                size_t candidate = (floatIndex + i) % floatPersistent_.size();
                if (!floatPersistent_.test(candidate)) {
                    floatPersistent_.set(candidate);
                    if (!stack_.empty())
                        stack_.back().usedFloatPersistentRegisters.set(candidate);
                    floatIndex = (candidate + 1) % floatPersistent_.size();
                    return FloatPersistentAtIndex(static_cast<int>(candidate));
                }
            }
        } else {
            size_t regCount = integerPersistent_.size();
            for (size_t i = 0; i < regCount - 1; i++) {
                size_t candidate = 1 + ((intIndex - 1 + i) % (regCount - 1));
                if (!integerPersistent_.test(candidate)) {
                    integerPersistent_.set(candidate);
                    if (!stack_.empty())
                        stack_.back().usedIntegerPersistentRegisters.set(candidate);
                    intIndex = 1 + ((candidate - 1 + 1) % (regCount - 1));
                    return IntegerPersistentAtIndex(static_cast<int>(candidate));
                }
            }
        }
        throw std::runtime_error("Out of persistent registers");
    }

    StackFrame &Context::CurrentFrame() {
        if (stack_.empty()) {
            throw std::runtime_error("No stack frame");
        }
        return stack_.back();
    }

    void Context::PushFrame(StackFrame &&frame) {
        stack_.push_back(std::move(frame));
    }

    void Context::PopScope(std::ostream &stream) {
        assert(stack_.size() >= 2 && "Attempted to pop scope from empty stack");

        // Any persistent registers used in the scope need to be saved back to the function root frame
        stack_.end()[-2].usedIntegerPersistentRegisters |= CurrentFrame().usedIntegerPersistentRegisters;
        stack_.end()[-2].usedFloatPersistentRegisters |= CurrentFrame().usedFloatPersistentRegisters;

        stack_.pop_back();
    }

    void Context::PopFrame() {
        assert(!stack_.empty() && "Attempted to pop frame from empty stack");
        stack_.pop_back();
    }

    void Context::PushScope() {
        assert(!stack_.empty() && "Attempted to push scope without frame on stack");
        stack_.push_back(stack_.back()); // Makes a copy
    }

    void Context::InsertFunction(const std::string &identifier, Function &&function) {
        functions_.emplace(identifier, function);
    }

    const Function &Context::GetFunction(const std::string &identifier) const {
        auto it = functions_.find(identifier);
        assert(it != functions_.end() && "Function not found in context");
        return it->second;
    }

    std::string Context::MakeLabel(const std::string &prefix) {
        std::string label{prefix};
        label.push_back('_');
        label.append(std::to_string(labelId_++));
        return label;
    }

    bool Context::IsGlobal(const std::string &identifier) const {
        bool isGlobal = globals_.find(identifier) != globals_.end();
        if (!isGlobal) return false;

        // Shadowing check
        return !(!stack_.empty() && CurrentBindings().Contains(identifier));
    }

    bool Context::IsArray(const std::string &identifier) const {
        // I like how we call is global first here, because it guarantees we are checking the correct one (shadowing)
        if (IsGlobal(identifier)) {
            auto it = globals_.find(identifier);
            return it != globals_.end() && it->second.IsArray();
        } else {
            return CurrentBindings().IsArray(identifier);
        }
    }

    void Context::InsertGlobal(const std::string &identifier, TypeSpecifier type) {
        globals_.emplace(identifier, type);
    }

    std::ostream &Context::DeferredRISC() {
        return deferredRISC_;
    }

    void Context::EmitDeferredRISC(std::ostream &stream) {
        stream << deferredRISC_.rdbuf();
    }

    TypeSpecifier Context::GetGlobalType(const std::string &identifier) const {
        auto itVar = globals_.find(identifier);
        if (itVar != globals_.end()) return itVar->second;
        throw std::runtime_error("Context::GetGlobalType Global not found in context");
    }

    void Context::InsertGlobalEnum(const std::string &identifier, const std::map<std::string, int> &values) {
        globalEnums_.Insert(identifier, values);
    }

    void Context::InsertGlobalEnum(const std::map<std::string, int> &values) {
        globalEnums_.Insert(values);
    }

    // Some duplication in finding where it is but it's ok
    int Context::LookupEnum(const std::string &identifier) const {
        // Try in scope first
        if (!stack_.empty()) {
            if (CurrentEnums().Contains(identifier)) {
                return CurrentEnums().Lookup(identifier);
            }
        }
        return globalEnums_.Lookup(identifier); // Will throw if not found
    }

    bool Context::IsEnum(const std::string &identifier) const {
        if (!stack_.empty()) {
            if (CurrentEnums().Contains(identifier)) {
                return true;
            }
        }
        return globalEnums_.Contains(identifier);
    }

    void Context::InsertGlobalStruct(const std::string &identifier,
                                     const std::vector<std::pair<std::string, TypeSpecifier>> &members) {
        globalStructs_.Insert(identifier, members);
    }

    std::vector<std::pair<std::string, TypeSpecifier>> Context::GetStruct(const std::string &identifier) const {
        // Try in local scope first
        if (!stack_.empty()) {
            if (CurrentStructs().Contains(identifier)) {
                return CurrentStructs().GetStruct(identifier);
            }
        }
        return globalStructs_.GetStruct(identifier); // Will throw if not found
    }

    bool Context::IsStruct(const std::string &identifier) const {
        if (!stack_.empty()) {
            if (CurrentStructs().Contains(identifier)) {
                return true;
            }
        }
        return globalStructs_.Contains(identifier);
    }

    static std::map<std::set<TypeSpecifier::Type>, TypeSpecifier::Type> aliasMap{
            {{TypeSpecifier::UNSIGNED, TypeSpecifier::INT},  TypeSpecifier::UNSIGNED},
            {{TypeSpecifier::INT,      TypeSpecifier::INT},  TypeSpecifier::INT}, // signed int
            {{TypeSpecifier::UNSIGNED, TypeSpecifier::CHAR}, TypeSpecifier::CHAR},
    };

    // Used for anonymous structs to have types at parsing stage
    TypeSpecifier Context::ResolveTypeAliasStatic(std::vector<TypeSpecifier> specifiers) {
        if (specifiers.size() == 1) {
            TypeSpecifier type = specifiers.front(); // Must copy
            return type;
        }
        std::set<TypeSpecifier::Type> typeSet{specifiers.begin(), specifiers.end()};
        auto it = aliasMap.find(typeSet);
        if (it == aliasMap.end()) {
            throw std::runtime_error("Unsupported type alias");
        }
        TypeSpecifier type = it->second; // Must copy
        return type;
    }

    TypeSpecifier Context::ResolveTypeAlias(std::vector<TypeSpecifier> specifiers) const {
        if (specifiers.size() == 1) {
            TypeSpecifier type = specifiers.front(); // Must copy
            if (type.IsStruct()) {
                // Do we have a definition
                if (IsStruct(type.GetStructIdentifier())) {
                    type.SetMembers(GetStruct(type.GetStructIdentifier()));
                } // Anonymous structs shouldn't need any additional handling here
            }
            return type;
        }
        std::set<TypeSpecifier::Type> typeSet{specifiers.begin(), specifiers.end()};
        auto it = aliasMap.find(typeSet);
        if (it == aliasMap.end()) {
            throw std::runtime_error("Unsupported type alias");
        }
        TypeSpecifier type = it->second; // Must copy
        // Never have to handle struct if there are multiple specifiers - you can't have unsigned struct etc.
        return type;
    }

    bool Context::SetEmitLHS(bool emitLHS) {
        bool old = emitLHS_;
        emitLHS_ = emitLHS;
        return old;
    }

    bool Context::EmitLHS() {
        return emitLHS_;
    }

    const Bindings &Context::CurrentBindings() const {
        assert(!stack_.empty() && "Attempted to get bindings from empty stack");
        return stack_.back().bindings;
    }

    const Enums &Context::CurrentEnums() const {
        assert(!stack_.empty() && "Attempted to get enums from empty stack");
        return stack_.back().enums;
    }

    const Structs &Context::CurrentStructs() const {
        assert(!stack_.empty() && "Attempted to get structs from empty stack");
        return stack_.back().structs;
    }

    Context::ScopedEmitLHS::ScopedEmitLHS(Context &context, bool newValue) :
            context_(context), oldValue_(context.SetEmitLHS(newValue)) {}

    Context::ScopedEmitLHS::~ScopedEmitLHS() {
        if (!released_)
            context_.SetEmitLHS(oldValue_);
    }

    void Context::ScopedEmitLHS::Release() {
        released_ = true;
        context_.SetEmitLHS(oldValue_);
    }

}
