#ifndef GETSET__HPP
#define GETSET__HPP

// Stereotype : GetSet
class GetSet {
    // Attributes
    public :
        type nom;
    private :
        /// com2
        bool boole;
    // Operations
    public :
        type getNom () const;
        void setNom (type value);
        bool isBoole () const;
        void setBoole (bool value);
};

#endif
