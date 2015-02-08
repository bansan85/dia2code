#ifndef ABSTRACTCLASS__HPP
#define ABSTRACTCLASS__HPP

class AbstractClass {
    // Operations
    public :
        virtual AbstractOperation () = 0;
        virtual VirtualOperation ();
        NonAbstractOperation ();
};

#endif
