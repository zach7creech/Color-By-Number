#pragma once
#include <vector>
#include <list>
#include <cstring>

namespace plank {

/** This is a class that implements disjoint sets, using "union by rank with path compression."
    It maintains some extra information that can be useful when you're using disjoint sets.
    Running times:

       - Initialize: O(n).
       - Union: O(1).
       - Find: O(alpha(n)), which is basically O(1).
       - Print: O(n)
       - Print_Equiv: O(n log n)
  */

class Disjoint_Set {
  public:
    void Initialize(int num_elements);
    int Union(int s1, int s2);             // s1 and s2 are set id's, not elements.  
                                           // Union() returns the set id of the new union.
    int Find(int element);                 // return the set id of the element.
    void Print() const;                    // Print all the information
    void Print_Equiv() const;              // Print the equivalence classes

    /* How many elements are in the disjoint set */

    size_t Size() const;

    /* Here you can get const pointers to the internals.  
       Sometimes this is useful.  See the protected entries for what these are.
     */
    
    const std::vector <int> *Get_Links() const;
    const std::vector <int> *Get_Ranks() const;
    const std::vector <int> *Get_Sizes() const;
    const std::vector <int> *Get_Set_Ids() const;
    const std::vector <int> *Get_Set_Id_Indices() const;
    const std::vector <std::list <int> > *Get_Elements() const;

  protected:
    std::vector <int> Links;          // Parent pointer in the set.  If you're the root, then -1,
                                      // and the set id of the set is your index.
    std::vector <int> Sizes;          // Only valid for roots.  It's the size of the set.
    std::vector <int> Ranks;          // Used for union-by-rank.

    std::vector <int> Set_Ids;        // A list of the roots, in no particular order.
    std::vector <int> Set_Id_Indices; // Only valid for roots. It is the root's index in Set_Ids.

    std::vector < std::list <int> > Elements;  // Only valid for roots. The elements of the set.
};

};
