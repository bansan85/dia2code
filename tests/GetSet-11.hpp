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
        void setNom (type value);
        constexpr type getNom ();
        void setBoole (bool value);
        constexpr bool isBoole ();
};

#endif
