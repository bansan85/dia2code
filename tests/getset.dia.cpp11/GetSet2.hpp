#ifndef GETSET2__HPP
#define GETSET2__HPP

class GetSet2 {
    // Attributes
    private :
        type nom;
        bool boole;
    // Operations
    public :
        constexpr type getNom ();
        void setNom (type value);
    private :
        constexpr bool isBoole ();
        void setBoole (bool value);
};

#endif
