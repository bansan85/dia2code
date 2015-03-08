#ifndef GETSET2__HPP
#define GETSET2__HPP

class GetSet2 {
    // Attributes
    private :
        type nom;
        bool boole;
    // Operations
    public :
        type getNom () const;
        void setNom (type value);
    private :
        bool isBoole () const;
        void setBoole (bool value);
};

#endif
