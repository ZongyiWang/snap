#ifndef TABLE_H
#define TABLE_H
#include "predicate.h"
#include "tmetric.h"
//#include "snap.h"

//#//////////////////////////////////////////////
/// Table class
class TTable;
class TTableContext;
typedef TPt<TTable> PTable;

/// Represent grouping key with IntV for integer and string attributes and FltV for float attributes.
typedef TPair<TIntV, TFltV> TGroupKey;

//TODO: move to separate file (map.h / file with PR and HITS) 
namespace TSnap {

  /// Get sequence of PageRank tables from given \c GraphSeq into \c TableSeq
  template <class PGraph>
  void MapPageRank(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq, 
      TTableContext& Context, const TStr& TableNamePrefix,
      const double& C, const double& Eps, const int& MaxIter);

  /// Get sequence of Hits tables from given \c GraphSeq into \c TableSeq
  template <class PGraph>
  void MapHits(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq, 
    TTableContext& Context, const TStr& TableNamePrefix,
    const int& MaxIter);
}

//#//////////////////////////////////////////////
/// Execution context
class TTableContext {
protected:
  TStrHash<TInt, TBigStrPool> StringVals; ///< StringPool - stores string data values and maps them to integers
  friend class TTable;
public:
  /// Default constructor
  TTableContext() {}
  /// Load TTableContext in binary from \c SIn
  TTableContext(TSIn& SIn): StringVals(SIn) {}
  /// Save TTableContext in binary to \c SOut
  void Save(TSOut& SOut) { StringVals.Save(SOut); }
};

//#//////////////////////////////////////////////
/// Table Row (Record)
class TTableRow {
protected:
  TIntV IntVals; ///< Values of the int columns for this row
  TFltV FltVals; ///< Values of the flt columns for this row
  TStrV StrVals; ///< Values of the str columns for this row
public:
  /// Default constructor
  TTableRow() {}
  /// Add int attribute to this row
  void AddInt(const TInt& Val) { IntVals.Add(Val); }
  /// Add float attribute to this row
  void AddFlt(const TFlt& Val) { FltVals.Add(Val); }
  /// Add string attribute to this row
  void AddStr(const TStr& Val) { StrVals.Add(Val); }
  /// Get int attributes of this row
  TIntV GetIntVals() const { return IntVals; }
  /// Get float attributes of this row
  TFltV GetFltVals() const { return FltVals; }
  /// Get string attributes of this row
  TStrV GetStrVals() const { return StrVals; }
};

/// Possible policies for aggregating node attributes
typedef enum {aaMin, aaMax, aaFirst, aaLast, aaMean, aaMedian, aaSum, aaCount} TAttrAggr;
/// Possible column-wise arithmetic operations
typedef enum {aoAdd, aoSub, aoMul, aoDiv, aoMod, aoMin, aoMax} TArithOp;

/// A table schema is a vector of pairs <attribute name, attribute type>
typedef TVec<TPair<TStr, TAttrType> > Schema; 

//#//////////////////////////////////////////////
/// Iterator class for TTable rows. ##Iterator
class TRowIterator{ 
  TInt CurrRowIdx; ///< Physical row index of current row pointed by iterator
  const TTable* Table; ///< Reference to table containing this row
public:
  /// Default constructor
  TRowIterator(): CurrRowIdx(0), Table(NULL) {}
  /// Construct iterator to row \c RowIds of \c TablePtr
  TRowIterator(TInt RowIdx, const TTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr) {}
  /// Copy constructor
  TRowIterator(const TRowIterator& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table) {}
  /// Increment iterator
  TRowIterator& operator++(int);
  /// Increment iterator (For Python compatibility)
  TRowIterator& Next(); 
  /// Check if this iterator points to a row that is before the one pointed by \c RowI
  bool operator < (const TRowIterator& RowI) const;
  /// Check if this iterator points to the same row pointed by \c RowI
  bool operator == (const TRowIterator& RowI) const;
  /// Get the id of the row pointed by this iterator
  TInt GetRowIdx() const;
  /// Return value of integer attribute specified by integer column index for current row
  TInt GetIntAttr(TInt ColIdx) const;
  /// Return value of floating point attribute specified by float column index for current row
  TFlt GetFltAttr(TInt ColIdx) const;
  /// Return value of string attribute specified by string column index for current row
  TStr GetStrAttr(TInt ColIdx) const;
  /// Return integer mapping of a string attribute value specified by string column index for current row
  TInt GetStrMap(TInt ColIdx) const;
  /// Return value of integer attribute specified by attribute name for current row
  TInt GetIntAttr(const TStr& Col) const;
  /// Return value of float attribute specified by attribute name for current row
  TFlt GetFltAttr(const TStr& Col) const;
  /// Return value of string attribute specified by attribute name for current row
  TStr GetStrAttr(const TStr& Col) const;  
  /// Return integer mapping of string attribute specified by attribute name for current row
  TInt GetStrMap(const TStr& Col) const;
};

//#//////////////////////////////////////////////
/// Iterator class for TTable rows, that allows logical row removal while iterating.
class TRowIteratorWithRemove {
  TInt CurrRowIdx; ///< Physical row index of current row pointer by iterator
  TTable* Table; ///< Reference to table containing this row
  TBool Start;	///< A flag indicating whether the current row in the first valid row of the table 
public:
  /// Default constructor
  TRowIteratorWithRemove(): CurrRowIdx(0), Table(NULL), Start(true) {}
  /// Construct iterator pointing to given row
  TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr);
  /// Construct iterator pointing to given row
  TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr, TBool IsStart) : CurrRowIdx(RowIdx), 
    Table(TablePtr), Start(IsStart) {}
  /// Copy constructor
  TRowIteratorWithRemove(const TRowIteratorWithRemove& RowI) : CurrRowIdx(RowI.CurrRowIdx), 
    Table(RowI.Table), Start(RowI.Start) {}
  /// Increment iterator
  TRowIteratorWithRemove& operator++(int);
  /// Increment iterator (For Python compatibility)
  TRowIteratorWithRemove& Next(); 
  /// Check if this iterator points to a row that is before the one pointed by \c RowI
  bool operator < (const TRowIteratorWithRemove& RowI) const;
  /// Check if this iterator points to the same row pointed by \c RowI
  bool operator == (const TRowIteratorWithRemove& RowI) const;
  /// Get physical index of current row
  TInt GetRowIdx() const;
  /// Get physical index of next row
  TInt GetNextRowIdx() const;
  /// Return value of integer attribute specified by integer column index for next row
  TInt GetNextIntAttr(TInt ColIdx) const;
  /// Return value of float attribute specified by float column index for next row
  TFlt GetNextFltAttr(TInt ColIdx) const;
  /// Return value of string attribute specified by string column index for next row
  TStr GetNextStrAttr(TInt ColIdx) const;   
  /// Return value of integer attribute specified by attribute name for next row
  TInt GetNextIntAttr(const TStr& Col) const;
  /// Return value of float attribute specified by attribute name for next row
  TFlt GetNextFltAttr(const TStr& Col) const;
  /// Return value of string attribute specified by attribute name for next row
  TStr GetNextStrAttr(const TStr& Col) const;   
  /// Checks whether iterator points to first valid row of the table
  TBool IsFirst() const;
  /// Remove next row
  void RemoveNext();
};

//#//////////////////////////////////////////////
/// Iterator over a vector of tables
class TTableIterator {
  TVec<PTable> PTableV; ///< Vector of TTables which are to be iterated over
  TInt CurrTableIdx; ///< Index of the current table pointed to by this iterator
public:
  /// Default constructor
  TTableIterator(TVec<PTable>& PTableV): PTableV(PTableV), CurrTableIdx(0) {}
  /// Return next table in the sequence and update iterator
  PTable Next() { return PTableV[CurrTableIdx++]; }
  /// Check if iterator has reached end of the sequence
  bool HasNext() { return CurrTableIdx < PTableV.Len(); }
};

//#//////////////////////////////////////////////
/// Table class: Relational table with columnar data storage
class TTable {
protected:
  static const TInt Last; ///< Special value for Next vector entry - last row in table
  static const TInt Invalid; ///< Special value for Next vector entry - logically removed row
public:
  TStr Name; ///< Table Name
protected:
  TTableContext& Context;  ///< Execution Context. ##TTable::Context
  Schema S; ///< Table Schema
  TCRef CRef;
  TInt NumRows; ///< Number of rows in the table (valid and invalid)
  TInt NumValidRows; ///< Number of valid rows in the table (i.e. rows that were not logically removed)
  TInt FirstValidRow; ///< Physical index of first valid row
  TInt LastValidRow; ///< Physical index of last valid row
  TIntV Next; ///< A vactor describing the logical order of the rows. ##TTable::Next
  TVec<TIntV> IntCols; ///< Data columns of integer attributes
  TVec<TFltV> FltCols; ///< Data columns of floating point attributes
  TVec<TIntV> StrColMaps; ///< Data columns of integer mappings of string attributes. ##TTable::StrColMaps
  THash<TStr,TPair<TAttrType,TInt> > ColTypeMap; /// A mapping from column name to column type and column index among columns of the same type.
  TStr IdColName; ///< Name of column associated with (optional) permanent row identifiers.
  TIntIntH RowIdMap; ///< Mapping of permanent row ids to physical id

  // Group mapping data structures
  THash<TStr, TPair<TStrV, TBool> > GroupStmtNames; ///< Maps user-given grouping statement names to their group-by attributes. ##TTable::GroupStmtNames
  THash<TPair<TStrV, TBool>, THash<TInt, TGroupKey> >GroupIDMapping; ///< Maps grouping statements to their (group id --> group-by key) mapping. ##TTable::GroupIDMapping
  THash<TPair<TStrV, TBool>, THash<TGroupKey, TIntV> >GroupMapping; ///< Maps grouping statements to their (group-by key --> group id) mapping. ##TTable::GroupMapping

  // Fields to be used when constructing a graph
  TStr SrcCol; ///< Column (attribute) to serve as src nodes when constructing the graph
  TStr DstCol; ///< Column (attribute) to serve as dst nodes when constructing the graph
  TStrV EdgeAttrV; ///< List of columns (attributes) to serve as edge attributes
  TStrV SrcNodeAttrV; ///< List of columns (attributes) to serve as source node attributes
  TStrV DstNodeAttrV; ///< List of columns (attributes) to serve as destination node attributes
  TStrTrV CommonNodeAttrs; ///< List of attribute pairs with values common to source and destination and their common given name. ##TTable::CommonNodeAttrs
  TVec<TIntV> RowIdBuckets; ///< Partitioning of row ids into buckets corresponding to different graph objects when generating a sequence of graphs.
  TInt CurrBucket; ///< Current row id bucket - used when generating a sequence of graphs using an iterator.
  TAttrAggr AggrPolicy; ///< Aggregation policy used for solving conflicts between different values of an attribute of the same node.

public:
  /***** value getters - getValue(column name, physical row Idx) *****/
  // no type checking. assuming ColName actually refers to the right type.
  /// Get the value of integer attribute \c ColName at row \c RowIdx
  TInt GetIntVal(const TStr& ColName, const TInt& RowIdx) { 
    return IntCols[ColTypeMap.GetDat(ColName).Val2][RowIdx]; 
  }
  /// Get the value of float attribute \c ColName at row \c RowIdx
  TFlt GetFltVal(const TStr& ColName, const TInt& RowIdx) { 
    return FltCols[ColTypeMap.GetDat(ColName).Val2][RowIdx]; 
  }
  /// Get the value of string attribute \c ColName at row \c RowIdx
  TStr GetStrVal(const TStr& ColName, const TInt& RowIdx) const { 
    return GetStrVal(ColTypeMap.GetDat(ColName).Val2, RowIdx); 
  }

  /// Get the schema of this table
  Schema GetSchema() { return S; }

/***** Utility functions *****/
protected:
  /// Add an integer column with name \c ColName
  void AddIntCol(const TStr& ColName);
  /// Add a float column with name \c ColName
  void AddFltCol(const TStr& ColName);
  /// Adds a label attribute with positive labels on selected rows and negative labels on the rest
  void ClassifyAux(const TIntV& SelectedRows, const TStr& LabelName, 
   const TInt& PositiveLabel = 1, const TInt& NegativeLabel=  0);

/***** Utility functions for handling string values *****/
  /// Get the value in column with id \c ColIdx at row \c RowIdx
  TStr GetStrVal(TInt ColIdx, TInt RowIdx) const { 
    return TStr(Context.StringVals.GetKey(StrColMaps[ColIdx][RowIdx])); 
  }
  /// Add \c Val in column with id \c ColIdx
  void AddStrVal(const TInt& ColIdx, const TStr& Val);
  /// Add \c Val in column with name \c Col
  void AddStrVal(const TStr& Col, const TStr& Val);

/***** Utility functions for handling Schema *****/
  /// Get name of the id column of this table
  TStr GetIdColName() const { return IdColName; }
  /// Get name of the column with index \c Idx in the schema
  TStr GetSchemaColName(TInt Idx) const { return S[Idx].Val1; }
  /// Get type of the column with index \c Idx in the schema
  TAttrType GetSchemaColType(TInt Idx) const { return S[Idx].Val2; }
  /// Add column with name \c ColName and type \c ColType to the schema
  void AddSchemaCol(const TStr& ColName, TAttrType ColType) { 
    S.Add(TPair<TStr,TAttrType>(ColName, ColType)); 
  }
  /// Get index of column \c ColName among columns of the same type in the schema
  TInt GetColIdx(const TStr& ColName) const { 
    return ColTypeMap.IsKey(ColName) ? ColTypeMap.GetDat(ColName).Val2 : TInt(-1); 
  }
  /// Check if \c Attr is an attribute of this table schema
  TBool IsAttr(const TStr& Attr);

/***** Utility functions for building graph from TTable *****/
  /// Add names of columns to be used as graph attributes
  void AddGraphAttribute(const TStr& Attr, TBool IsEdge, TBool IsSrc, TBool IsDst);
  /// Add vector of names of columns to be used as graph attributes
  void AddGraphAttributeV(TStrV& Attrs, TBool IsEdge, TBool IsSrc, TBool IsDst);
  /// Check if given \c NodeId is seen earlier; if not, add it to \c Graph and hashmap \c NodeVals
  void CheckAndAddIntNode(PNEANet Graph, THashSet<TInt>& NodeVals, TInt NodeId);

  /// Check if given \c NodeVal is seen earlier; if not, add it to \c Graph and hashmap \c NodeVals
  template<class T>
  TInt CheckAndAddFltNode(T Graph, THash<TFlt, TInt>& NodeVals, TFlt FNodeVal) {
    if (!NodeVals.IsKey(FNodeVal)) {
      TInt NodeVal = NodeVals.Len();
      Graph->AddNode(NodeVal);
      NodeVals.AddKey(FNodeVal);
      NodeVals.AddDat(FNodeVal, NodeVal);
      return NodeVal;
    } else { return NodeVals.GetDat(FNodeVal); }
  }
  /// Add attributes of edge corresponding to \c RowId to the \c Graph
  void AddEdgeAttributes(PNEANet& Graph, int RowId);
  /// Takes as parameters, and updates, maps NodeXAttrs: Node Id --> (attribute name --> Vector of attribute values)
  void AddNodeAttributes(TInt NId, TStrV NodeAttrV, TInt RowId, 
   THash<TInt, TStrIntVH>& NodeIntAttrs, THash<TInt, TStrFltVH>& NodeFltAttrs, 
   THash<TInt, TStrStrVH>& NodeStrAttrs);
  /// Make a single pass over the rows in the given row id set, and creates nodes, edges, assigns node and edge attributes
  PNEANet BuildGraph(const TIntV& RowIds, TAttrAggr AggrPolicy);
  /// Initialize the RowIdBuckets vector which will be used for the graph sequence creation.
  void InitRowIdBuckets(int NumBuckets);
  /// Fill RowIdBuckets with sets of row ids ##TTable::FillBucketsByWindow
  void FillBucketsByWindow(TStr SplitAttr, TInt JumpSize, TInt WindowSize, 
   TInt StartVal, TInt EndVal);
  /// Fill RowIdBuckets with sets of row ids ##TTable::FillBucketsByInterval
  void FillBucketsByInterval(TStr SplitAttr, TIntPrV SplitIntervals);
  /// Return a sequence of graphs ##TTable::GetGraphsFromSequence
  TVec<PNEANet> GetGraphsFromSequence(TAttrAggr AggrPolicy);
  /// Return the first graph of the sequence ##TTable::GetFirstGraphFromSequence
  PNEANet GetFirstGraphFromSequence(TAttrAggr AggrPolicy);
  /// Return the next graph in sequence corresponding to RowIdBuckets ##TTable::GetNextGraphFromSequence
  PNEANet GetNextGraphFromSequence();

  /// Aggregate vector into a single scalar value according to a policy ##TTable::AggregateVector
  template <class T> 
  T AggregateVector(TVec<T>& V, TAttrAggr Policy) {
    switch (Policy) {
      case aaMin: {
        T Res = V[0];
        for (TInt i = 1; i < V.Len(); i++) {
          if (V[i] < Res) { Res = V[i]; }
        }
        return Res;
      }
      case aaMax: {
        T Res = V[0];
        for (TInt i = 1; i < V.Len(); i++) {
          if (V[i] > Res) { Res = V[i]; }
        }
        return Res;
      }
      case aaFirst: {
        return V[0];
      }
      case aaLast:{
        return V[V.Len()-1];
      }
      case aaSum: {
        T Res = V[0];
        for (TInt i = 1; i < V.Len(); i++) {
          Res = Res + V[i];
        }
        return Res;
      }
      case aaMean: {
        T Res = V[0];
        for (TInt i = 1; i < V.Len(); i++) {
          Res = Res + V[i];
        }
        //Res = Res / V.Len(); // TODO: Handle Str case separately?
        return Res;
      }
      case aaMedian: {
        V.Sort();
        return V[V.Len()/2];
      }
      case aaCount: {
        // NOTE: Code should never reach here
        // I had to put this here to avoid a compiler warning
        // Is there a better way to do this?
        return V[0];
      }
    }
    // added to remove a compiler warning
    T ShouldNotComeHere;
    return ShouldNotComeHere;
  }

  /***** Grouping Utility functions *************/
  /// Group/hash by a single column with integer values. ##TTable::GroupByIntCol
  void GroupByIntCol(const TStr& GroupBy, THash<TInt,TIntV>& grouping, 
   const TIntV& IndexSet, TBool All) const; 
  /// Group/hash by a single column with float values. Returns hash table with grouping.
  void GroupByFltCol(const TStr& GroupBy, THash<TFlt,TIntV>& grouping, 
   const TIntV& IndexSet, TBool All) const;
  /// Group/hash by a single column with string values. Returns hash table with grouping.
  void GroupByStrCol(const TStr& GroupBy, THash<TInt,TIntV>& grouping, 
   const TIntV& IndexSet, TBool All) const;

  /// Template for utility function to update a grouping hash map
  template <class T>
  void UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val) const{
    if (Grouping.IsKey(Key)) {
      Grouping.GetDat(Key).Add(Val);
    } else {
      TIntV NewGroup;
      NewGroup.Add(Val);
      Grouping.AddDat(Key, NewGroup);
    }
  }

  /***** Utility functions for sorting by columns *****/
  /// Returns positive value if R1 is bigger, negative value if R2 is bigger, and 0 if they are equal (strcmp semantics)
  inline TInt CompareRows(TInt R1, TInt R2, const TAttrType& CompareByType, 
   const TInt& CompareByIndex, TBool Asc = true);
  /// Returns positive value if R1 is bigger, negative value if R2 is bigger, and 0 if they are equal (strcmp semantics)
  inline TInt CompareRows(TInt R1, TInt R2, const TVec<TAttrType>& CompareByTypes, 
   const TIntV& CompareByIndices, TBool Asc = true); 
  /// Get pivot element for QSort.
  TInt GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, 
   const TIntV& SortByIndices, TBool Asc);
  /// Partition vector for QSort
  TInt Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes,
   const TIntV& SortByIndices, TBool Asc);
  /// Perform insertion sort on given vector \c V
  void ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, 
   const TIntV& SortByIndices, TBool Asc = true);
  /// Perform QSort on given vector \c V
  void QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, 
   const TIntV& SortByIndices, TBool Asc = true);

  /// Check if \c RowIdx corresponds to a valid (i.e. not deleted) row
  bool IsRowValid(TInt RowIdx) const{ return Next[RowIdx] != Invalid;}
  /// Get the id of the last valid row of the table
  TInt GetLastValidRowIdx();

/***** Utility functions for removing rows (not through iterator) *****/
  /// Remove first valid row of the table
  void RemoveFirstRow();
  /// Remove row with id \c RowIdx
  void RemoveRow(TInt RowIdx);
  /// Remove rows with ids present in \c RemoveV
  void RemoveRows(const TIntV& RemoveV);
  /// Remove all rows that are not mentioned in the SORTED vector \c KeepV
  void KeepSortedRows(const TIntV& KeepV);

/***** Utility functions for Join *****/
  /// Initialize an empty table for the join of this table with the given table
  PTable InitializeJointTable(const TTable& Table);
  /// Add joint row T1[RowIdx1]<=>T2[RowIdx2]
  void AddJointRow(const TTable& T1, const TTable& T2, TInt RowIdx1, TInt RowIdx2);

  /// Resize the table to hold \c RowCount rows
  void ResizeTable(int RowCount);
  /// Get the start index to a chunk of empty rows of size \c NewRows
  int GetEmptyRowsStart(int NewRows);
  /// Add rows from \c Table that correspond to ids in \c RowIDs
  void AddSelectedRows(const TTable& Table, const TIntV& RowIDs);
  /// Add \c NewRows rows from the given vectors for each column type
  void AddNRows(int NewRows, const TVec<TIntV>& IntColsP, const TVec<TFltV>& FltColsP, 
   const TVec<TIntV>& StrColMapsP);
  /// Update table state after adding one or more rows
  void UpdateTableForNewRow();

public:
/***** Constructors *****/
  TTable(); 
  TTable(TTableContext& Context);
  TTable(const TStr& TableName, const Schema& S, TTableContext& Context);
  TTable(TSIn& SIn, TTableContext& Context);

  /// Constructor to build table out of a hash table of int->int
  TTable(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, 
   const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false);
  /// Constructor to build table out of a hash table of int->float
  TTable(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, 
   const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false);
  // TTable(const TStr& TableName, const THash<TInt,TStr>& H, const TStr& Col1, 
  //  const TStr& Col2, TTableContext& Context);
  
  /// Copy constructor
  TTable(const TTable& Table): Name(Table.Name), Context(Table.Context), S(Table.S),
    NumRows(Table.NumRows), NumValidRows(Table.NumValidRows), FirstValidRow(Table.FirstValidRow),
    Next(Table.Next), IntCols(Table.IntCols), FltCols(Table.FltCols),
    StrColMaps(Table.StrColMaps), ColTypeMap(Table.ColTypeMap), 
    GroupMapping(Table.GroupMapping),
    SrcCol(Table.SrcCol), DstCol(Table.DstCol),
    EdgeAttrV(Table.EdgeAttrV), SrcNodeAttrV(Table.SrcNodeAttrV),
    DstNodeAttrV(Table.DstNodeAttrV), CommonNodeAttrs(Table.CommonNodeAttrs) {} 

  TTable(const TTable& Table, const TIntV& RowIds);

  static PTable New() { return new TTable(); }
  static PTable New(TTableContext& Context) { return new TTable(Context); }
  static PTable New(const TStr& TableName, const Schema& S, TTableContext& Context) { 
    return new TTable(TableName, S, Context); 
  }
  /// Return pointer to a table constructed from given int->int hash
  static PTable New(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, 
   const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false) {
    return new TTable(TableName, H, Col1, Col2, Context, IsStrKeys);
  }
  /// Return pointer to a table constructed from given int->float hash
  static PTable New(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, 
   const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false) {
    return new TTable(TableName, H, Col1, Col2, Context, IsStrKeys);
  }
  /// Return pointer to a new table created from given \c Table
  static PTable New(const PTable Table) { return new TTable(*Table); }
  /// Return pointer to a new table created from given \c Table, with name set to \c TableName
  static PTable New(const PTable Table, const TStr& TableName) { 
    PTable T = New(Table); T->Name = TableName; 
    return T; 
  }
  /// Return pointer to a new table created from the rows \c RowIds of given \c Table
  static PTable New(const PTable Table, const TStr& TableName, const TIntV& RowIds) { 
    PTable T = new TTable(*Table, RowIds); 
    T->Name = TableName; 
    return T;
  }

/***** Save / Load functions *****/
  /// Load table from spread sheet (TSV, CSV, etc)
  static PTable LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, 
   TTableContext& Context, const char& Separator = '\t', TBool HasTitleLine = false);
  /// Load table from spread sheet - but only load the columns specified by RelevantCols
  static PTable LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, 
   TTableContext& Context, const TIntV& RelevantCols, const char& Separator = '\t', 
   TBool HasTitleLine = false);
  /// Load table from spread sheet - compact prototype (tab-separated input file, has title line, anonymous table)
  static PTable LoadSS(const Schema& S, const TStr& InFnm, TTableContext& Context){
    return LoadSS(TStr(), S, InFnm, Context, '\t', true);
  }
  /// Save table schema + content into a TSV file
  void SaveSS(const TStr& OutFNm);
  /// Save table schema + content into a binary
  void SaveBin(const TStr& OutFNm);
  /// Load table from binary. ##TTable::Load
  static PTable Load(TSIn& SIn, TTableContext& Context){ return new TTable(SIn, Context);} 
  /// Save table schema + content into binary. ##TTable::Save
  void Save(TSOut& SOut);

  /// Build table from hash table of int->int
  static PTable TableFromHashMap(const TStr& TableName, const THash<TInt,TInt>& H, 
   const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false) {
    PTable T = New(TableName, H, Col1, Col2, Context, IsStrKeys);
    T->InitIds();
    return T;
  }
  /// Build table from hash table of int->float
  static PTable TableFromHashMap(const TStr& TableName, const THash<TInt,TFlt>& H, 
   const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false) {
    PTable T = New(TableName, H, Col1, Col2, Context, IsStrKeys);
    T->InitIds();
    return T;
  }

  // Make a new TTable from the subset of physical indices of the rows of current table
  // RS 11/26/13 commented out CopySubset(), it is not implemented
  // PTable CopySubset(const TIntV& RowIds) const;
  
/***** Graph handling *****/
  /// Create a network out of this table
  PNEANet ToGraph(TAttrAggr AggrPolicy);
  /// Create a directed graph out of this table
  PNGraph ToGraphDirected(TAttrAggr AggrPolicy);
  /// Create an undirected graph out of this table
  PUNGraph ToGraphUndirected(TAttrAggr AggrPolicy);
  
  /// Create a sequence of graphs based on values of column SplitAttr and windows specified by JumpSize and WindowSize.
  TVec<PNEANet> ToGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, 
    TInt WindowSize, TInt JumpSize, TInt StartVal = TInt::Mn, TInt EndVal = TInt::Mx);
  /// Create a sequence of graphs based on values of column SplitAttr and intervals specified by SplitIntervals.
  TVec<PNEANet> ToVarGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals);
  /// Create a sequence of graphs based on grouping specified by GroupAttr
  TVec<PNEANet> ToGraphPerGroup(TStr GroupAttr, TAttrAggr AggrPolicy);

  /// Create the graph sequence one at a time ##TTable::ToGraphSequenceIterator
  PNEANet ToGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, 
    TInt WindowSize, TInt JumpSize, TInt StartVal = TInt::Mn, TInt EndVal = TInt::Mx);
  /// Create the graph sequence one at a time ##TTable::ToVarGraphSequenceIterator
  PNEANet ToVarGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals);
  /// Create the graph sequence one at a time ##TTable::ToGraphPerGroupIterator
  PNEANet ToGraphPerGroupIterator(TStr GroupAttr, TAttrAggr AggrPolicy);
  /// Calls to this must be preceded by a call to one of the above ToGraph*Iterator functions
  PNEANet NextGraphIterator();
  /// Check if the end of the graph sequence is reached
  TBool IsLastGraphOfSequence();

  /// Get the name of the column to be used as src nodes in the graph
	TStr GetSrcCol() const { return SrcCol; }
  /// Set the name of the column to be used as src nodes in the graph
  void SetSrcCol(const TStr& Src) {
    if (!ColTypeMap.IsKey(Src)) { TExcept::Throw(Src + ": no such column"); }
    SrcCol = Src;
  }
  /// Get the name of the column to be used as dst nodes in the graph
	TStr GetDstCol() const { return DstCol; }
  /// Set the name of the column to be used as dst nodes in the graph
  void SetDstCol(const TStr& Dst) {
    if (!ColTypeMap.IsKey(Dst)) { TExcept::Throw(Dst + ": no such column"); }
    DstCol = Dst;
  }
  /// Specify column to be used as graph edge attribute
  void AddEdgeAttr(const TStr& Attr) { AddGraphAttribute(Attr, true, false, false); }
  /// Specify columns to be used as graph edge attributes
  void AddEdgeAttr(TStrV& Attrs) { AddGraphAttributeV(Attrs, true, false, false); }
  /// Specify column to be used as src node atribute of the graph
  void AddSrcNodeAttr(const TStr& Attr) { AddGraphAttribute(Attr, false, true, false); }
  /// Specify columns to be used as src node attributes of the graph
  void AddSrcNodeAttr(TStrV& Attrs) { AddGraphAttributeV(Attrs, false, true, false); }
  /// Specify column to be used as dst node atribute of the graph
  void AddDstNodeAttr(const TStr& Attr) { AddGraphAttribute(Attr, false, false, true); }
  /// Specify columns to be used as dst node attributes of the graph
  void AddDstNodeAttr(TStrV& Attrs) { AddGraphAttributeV(Attrs, false, false, true); }
  /// Handling the common case where src and dst both belong to the same "universe" of entities
  void AddNodeAttr(const TStr& Attr) { AddSrcNodeAttr(Attr); AddDstNodeAttr(Attr); }
  /// Handling the common case where src and dst both belong to the same "universe" of entities
  void AddNodeAttr(TStrV& Attrs) { AddSrcNodeAttr(Attrs); AddDstNodeAttr(Attrs); }
  /// Set the columns to be used as both src and dst node attributes
  void SetCommonNodeAttrs(const TStr& SrcAttr, const TStr& DstAttr, const TStr& CommonAttrName){ 
    CommonNodeAttrs.Add(TStrTr(SrcAttr, DstAttr, CommonAttrName));
  }
  /// Get src node int attribute name vector
	TStrV GetSrcNodeIntAttrV() const;
  /// Get dst node int attribute name vector
  TStrV GetDstNodeIntAttrV() const;
  /// Get edge int attribute name vector
	TStrV GetEdgeIntAttrV() const;
  /// Get src node float attribute name vector
	TStrV GetSrcNodeFltAttrV() const;
  /// Get dst node float attribute name vector
  TStrV GetDstNodeFltAttrV() const;
  /// Get edge float attribute name vector
	TStrV GetEdgeFltAttrV() const;
  /// Get src node str attribute name vector
	TStrV GetSrcNodeStrAttrV() const;
  /// Get dst node str attribute name vector
  TStrV GetDstNodeStrAttrV() const;
  /// Get edge str attribute name vector
	TStrV GetEdgeStrAttrV() const;

  /// Extract node TTable from PNEANet
  static PTable GetNodeTable(const PNEANet& Network, const TStr& TableName, TTableContext& Context);
  /// Extract edge TTable from PNEANet
  static PTable GetEdgeTable(const PNEANet& Network, const TStr& TableName, TTableContext& Context);

  /// Extract node and edge property TTables from THash
  static PTable GetFltNodePropertyTable(const PNEANet& Network, const TStr& TableName, 
   const TIntFltH& Property, const TStr& NodeAttrName, const TAttrType& NodeAttrType, 
   const TStr& PropertyAttrName, TTableContext& Context);

/***** Basic Getters *****/
  /// Get type of column \c ColName
	TAttrType GetColType(const TStr& ColName) const{ return ColTypeMap.GetDat(ColName).Val1; };
  /// Get total number of rows in this table
  TInt GetNumRows() const { return NumRows;}
  /// Get number of valid, i.e. not deleted, rows in this table
  TInt GetNumValidRows() const { return NumValidRows;}

  /// Get a map of logical to physical row ids
  THash<TInt, TInt> GetRowIdMap() const { return RowIdMap;}
	
/***** Iterators *****/
  /// Get iterator to the first valid row of the table
  TRowIterator BegRI() const { return TRowIterator(FirstValidRow, this);}
  /// Get iterator to the last valid row of the table
  TRowIterator EndRI() const { return TRowIterator(TTable::Last, this);}
  /// Get iterator with reomve to the first valid row
  TRowIteratorWithRemove BegRIWR(){ return TRowIteratorWithRemove(FirstValidRow, this);}
  /// Get iterator with reomve to the last valid row
  TRowIteratorWithRemove EndRIWR(){ return TRowIteratorWithRemove(TTable::Last, this);}

/***** Table Operations *****/
	/// Add a label to a column
	void AddLabel(const TStr& Column, const TStr& NewLabel);
  /// Rename a column
  void Rename(const TStr& Column, const TStr& NewLabel);

	/// Remove rows with duplicate values in given column
  void Unique(const TStr& Col);
  /// Remove rows with duplicate values in given columns
	void Unique(const TStrV& Cols, TBool Ordered = true);

	/// Select ##TTable::Select
	void Select(TPredicate& Predicate, TIntV& SelectedRows, TBool Remove = true);
  void Select(TPredicate& Predicate) {
    TIntV SelectedRows;
    Select(Predicate, SelectedRows, true);
  }
  void Classify(TPredicate& Predicate, const TStr& LabelName, const TInt& PositiveLabel = 1, 
   const TInt& NegativeLabel = 0);

  /// Select atomic ##TTable::SelectAtomic
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, 
   TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp) {
    TIntV SelectedRows;
    SelectAtomic(Col1, Col2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, 
   const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  void SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, 
   TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp) {
    TIntV SelectedRows;
    SelectAtomicIntConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, 
   const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  void SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp,
   TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp) {
    TIntV SelectedRows;
    SelectAtomicStrConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp, 
   const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  void SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, 
   TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp) {
    TIntV SelectedRows;
    SelectAtomicFltConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, 
   const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  /// Store column for a group. Physical row ids have to be passed
  void StoreGroupCol(const TStr& GroupColName, const TVec<TPair<TInt, TInt> >& GroupAndRowIds);

  /// Helper function for grouping ##TTable::GroupAux
  void GroupAux(const TStrV& GroupBy, THash<TGroupKey, TPair<TInt, TIntV> >& Grouping, 
   TBool Ordered, const TStr& GroupColName, TBool KeepUnique, TIntV& UniqueVec);

  /// Group rows depending on values of \c GroupBy columns ##TTable::Group
  void Group(const TStrV& GroupBy, const TStr& GroupColName, TBool Ordered = true);
  
  /// Count number of unique elements ##TTable::Count
  void Count(const TStr& CountColName, const TStr& Col);

  /// Order the rows according to the values in columns of OrderBy (in descending lexicographic order). 
  void Order(const TStrV& OrderBy, const TStr& OrderColName = "", TBool ResetRankByMSC = false, TBool Asc = true);
  
  /// Aggregate values of ValAttr after grouping with respect to GroupByAttrs. Result stored as new attribute ResAttr
  void Aggregate(const TStrV& GroupByAttrs, TAttrAggr AggOp, const TStr& ValAttr,
   const TStr& ResAttr, TBool Ordered = true);

  /// Aggregate attributes in AggrAttrs across columns
  void AggregateCols(const TStrV& AggrAttrs, TAttrAggr AggOp, const TStr& ResAttr);

  /// Splice table into subtables according to a grouping statement
  TVec<PTable> SpliceByGroup(const TStrV& GroupByAttrs, TBool Ordered = true);

  /// Perform equijoin ##TTable::Join
  PTable Join(const TStr& Col1, const TTable& Table, const TStr& Col2);
  PTable Join(const TStr& Col1, const PTable& Table, const TStr& Col2) { 
    return Join(Col1, *Table, Col2); 
  }
  /// Join table with itself, on values of \c Col
  PTable SelfJoin(const TStr& Col) { return Join(Col, *this, Col); }

  /// Select first N rows from the table
  void SelectFirstNRows(const TInt& N);

	// compute distances between elements in this->Col1 and Table->Col2 according
	// to given metric. Store the distances in DistCol, but keep only rows where
	// distance <= threshold
	// void Dist(const TStr& Col1, const TTable& Table, const TStr Col2, const TStr& DistColName, 
  //  const TMetric& Metric, TFlt threshold);

  /// Release memory of deleted rows, and defrag ##TTable::Defrag
  void Defrag();

  /// Add entire int column to table
  void StoreIntCol(const TStr& ColName, const TIntV& ColVals);
  /// Add entire flt column to table
  void StoreFltCol(const TStr& ColName, const TFltV& ColVals);
  /// Add entire str column to table
  void StoreStrCol(const TStr& ColName, const TStrV& ColVals);
  
  /// Add all the rows of the input table. Allows duplicate rows (not a union)
  void AddTable(const TTable& T);
  /// Append all rows of \c T to this table, and recalcutate indices.
  void ConcatTable(const PTable& T) {AddTable(*T); Reindex(); }
  
  /// Add row corresponding to \c RI
  void AddRow(const TRowIterator& RI);
  /// Add row with values corresponding to the given vectors by type
  void AddRow(const TIntV& IntVals, const TFltV& FltVals, const TStrV& StrVals);
  /// Add row with values taken from given TTableRow
  void AddRow(const TTableRow& Row) { AddRow(Row.GetIntVals(), Row.GetFltVals(), Row.GetStrVals()); };
  /// Get set of row ids of rows common with table \c T
  void GetCollidingRows(const TTable& T, THashSet<TInt>& Collisions);

  /// Return union of this table with given \c Table
  PTable Union(const TTable& Table, const TStr& TableName);
  PTable Union(const PTable& Table, const TStr& TableName) { return Union(*Table, TableName); };
  /// Return union of this table with given \c Table, preserving duplicates
  PTable UnionAll(const TTable& Table, const TStr& TableName);
  PTable UnionAll(const PTable& Table, const TStr& TableName) { return UnionAll(*Table, TableName); };
  /// Same as TTable::ConcatTable
  void UnionAllInPlace(const TTable& Table);
  void UnionAllInPlace(const PTable& Table) { return UnionAllInPlace(*Table); };
  /// Return intersection of this table with given \c Table
  PTable Intersection(const TTable& Table, const TStr& TableName);
  PTable Intersection(const PTable& Table, const TStr& TableName) { return Intersection(*Table, TableName); };
  /// Return table with rows that are present in this table but not in given \c Table
  PTable Minus(TTable& Table, const TStr& TableName);
  PTable Minus(const PTable& Table, const TStr& TableName) { return Minus(*Table, TableName); };
  /// Return table with only the columns in \c ProjectCols
  PTable Project(const TStrV& ProjectCols, const TStr& TableName);
  /// Keep only the columns specified in \c ProjectCols
  void ProjectInPlace(const TStrV& ProjectCols);
  
  /* Column-wise arithmetic operations */

  /// Performs columnwise arithmetic operation ##TTable::ColGenericOp
  void ColGenericOp(const TStr& Attr1, const TStr& Attr2, const TStr& ResAttr, TArithOp op);
  /// Performs columnwise addition. See TTable::ColGenericOp
  void ColAdd(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise subtraction. See TTable::ColGenericOp
  void ColSub(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise multiplication. See TTable::ColGenericOp
  void ColMul(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise division. See TTable::ColGenericOp
  void ColDiv(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise modulus. See TTable::ColGenericOp
  void ColMod(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs min of two columns. See TTable::ColGenericOp
  void ColMin(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs max of two columns. See TTable::ColGenericOp
  void ColMax(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");

  /// Performs columnwise arithmetic operation with column of given table.
  void ColGenericOp(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr, 
    TArithOp op, TBool AddToFirstTable);
  /// Performs columnwise addition with column of given table.
  void ColAdd(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise subtraction with column of given table.
  void ColSub(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise multiplication with column of given table.
  void ColMul(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise division with column of given table.
  void ColDiv(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise modulus with column of given table.
  void ColMod(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);

  /// Performs arithmetic op of column values and given \c Num
  void ColGenericOp(const TStr& Attr1, const TFlt& Num, const TStr& ResAttr, TArithOp op, const TBool floatCast);
  /// Performs addition of column values and given \c Num
  void ColAdd(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs subtraction of column values and given \c Num
  void ColSub(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs multiplication of column values and given \c Num
  void ColMul(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs division of column values and given \c Num
  void ColDiv(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs modulus of column values and given \c Num
  void ColMod(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);

  /* Column-wise string operations */

  /// Concatenates two string columns.
  void ColConcat(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Concatenates string column with column of given table.
  void ColConcat(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Concatenates column values with given string value.
  void ColConcat(const TStr& Attr1, const TStr& Val, const TStr& ResultAttrName="");

  /// Read values of entire int column into \c Result
  void ReadIntCol(const TStr& ColName, TIntV& Result) const;
  /// Read values of entire float column into \c Result
  void ReadFltCol(const TStr& ColName, TFltV& Result) const;
  /// Read values of entire string column into \c Result
  void ReadStrCol(const TStr& ColName, TStrV& Result) const;

  /// Add explicit row ids, initialize hash set mapping ids to physical rows
  void InitIds();
  /// Reinitialize row ids
  void Reindex();

  /// Add a column of explicit integer identifiers to the rows
  void AddIdColumn(const TStr& IdColName);

  /// Distance based filter ##TTable::IsNextK
  PTable IsNextK(const TStr& OrderCol, TInt K, const TStr& GroupBy, const TStr& RankColName = "");

  /// Debug: print sizes of various fields of table
  void PrintSize();

  /// Get sequence of PageRank tables from given \c GraphSeq
  static TTableIterator GetMapPageRank(const TVec<PNEANet>& GraphSeq, 
    TTableContext& Context, const TStr& TableNamePrefix = "PageRankTable",
    const double& C = 0.85, const double& Eps = 1e-4, const int& MaxIter = 100) {
    TVec<PTable> TableSeq(GraphSeq.Len());
    TSnap::MapPageRank(GraphSeq, TableSeq, Context, TableNamePrefix, C, Eps, MaxIter);
    return TTableIterator(TableSeq);
  }

  /// Get sequence of Hits tables from given \c GraphSeq
  static TTableIterator GetMapHitsIterator(const TVec<PNEANet>& GraphSeq, 
    TTableContext& Context, const TStr& TableNamePrefix = "HitsTable",
    const int& MaxIter = 20) {
    TVec<PTable> TableSeq(GraphSeq.Len());
    TSnap::MapHits(GraphSeq, TableSeq, Context, TableNamePrefix, MaxIter);
    return TTableIterator(TableSeq);
  }

  friend class TPt<TTable>;
  friend class TRowIterator;
  friend class TRowIteratorWithRemove;
};

typedef TPair<TStr,TAttrType> TStrTypPr;

namespace TSnap {

  /// Get sequence of PageRank tables from given \c GraphSeq into \c TableSeq
  template <class PGraph>
  void MapPageRank(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq, 
    TTableContext& Context, const TStr& TableNamePrefix,
    const double& C, const double& Eps, const int& MaxIter) {
    int NumGraphs = GraphSeq.Len();
    TableSeq.Reserve(NumGraphs, NumGraphs);
    // this loop is parallelizable
    for (TInt i = 0; i < NumGraphs; i++){
      TIntFltH PRankH;
      GetPageRank(GraphSeq[i], PRankH, C, Eps, MaxIter);
      TableSeq[i] = TTable::TableFromHashMap(TableNamePrefix + "_" + i.GetStr(), 
        PRankH, "NodeId", "PageRank", Context, false);
    }
  }

  /// Get sequence of Hits tables from given \c GraphSeq into \c TableSeq
  template <class PGraph>
  void MapHits(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq, 
    TTableContext& Context, const TStr& TableNamePrefix,
    const int& MaxIter) {
    int NumGraphs = GraphSeq.Len();
    TableSeq.Reserve(NumGraphs, NumGraphs);
    // this loop is parallelizable
    for (TInt i = 0; i < NumGraphs; i++){
      TIntFltH HubH;
      TIntFltH AuthH;
      GetHits(GraphSeq[i], HubH, AuthH, MaxIter);
      PTable HubT =  TTable::TableFromHashMap("1", HubH, "NodeId", "Hub", Context, false);
      PTable AuthT =  TTable::TableFromHashMap("2", AuthH, "NodeId", "Authority", Context, false);
      PTable HitsT = HubT->Join("NodeId", AuthT, "NodeId");
      HitsT->Rename("1.NodeId", "NodeId");
      HitsT->Rename("1.Hub", "Hub");
      HitsT->Rename("2.Authority", "Authority");
      TStrV V = TStrV(3, 0);
      V.Add("NodeId");
      V.Add("Hub");
      V.Add("Authority");
      HitsT->ProjectInPlace(V);
      HitsT->Name = TableNamePrefix + "_" + i.GetStr();
      TableSeq[i] = HitsT;
    }
  }
}

#endif //TABLE_H
