#pragma once
 
#include "common.h"
#include "formula.h"
 
#include <functional>
#include <optional>
#include <set>
#include <stack>
#include <unordered_set>

class Sheet;
 
class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();
 
    void Set(std::string text);
    void Clear();
 
    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    
    bool IsReferenced() const;
    void InvalidateAllCache(bool flag);
    
private:
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const;
        virtual bool HasCache();
        virtual void InvalidateCache();
    };
    
    class EmptyImpl : public Impl {
    public:    
        Value GetValue() const override;
        std::string GetText() const override;      
    };
    
    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text); 
        Value GetValue() const override;       
        std::string GetText() const override;
        
    private:
        std::string text_;        
    };
    
    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string text, SheetInterface& sheet);
        Value GetValue() const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;
        bool HasCache() override;
        void InvalidateCache() override;
        
    private:
        mutable std::optional<FormulaInterface::Value> cache_;
        std::unique_ptr<FormulaInterface> formula_ptr_;
        SheetInterface& sheet_;
    };
    
    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;
    std::set<Cell*> dependent_cells_;
    std::set<Cell*> referenced_cells_;

    void FindCircularDepend(std::unique_ptr<Impl> &temp_impl);
    void UpdateDepend();
};