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
        constexpr type getNom ();
        void setNom (type value);
        constexpr bool isBoole ();
        void setBoole (bool value);
};

#endif
