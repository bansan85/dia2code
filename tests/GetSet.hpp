#ifndef GETSET__HPP
#define GETSET__HPP

// Stereotype : GetSet
/** \class GetSet
*/
class GetSet {
    // Attributes
    public :
        type nom;
    private :
        /// com2
        bool boole;
    // Operations
    public :
        void setNom (type value);
        type getNom () const;
        void setBoole (bool value);
        bool isBoole () const;
};

#endif
