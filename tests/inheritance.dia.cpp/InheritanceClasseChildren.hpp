#ifndef INHERITANCECLASSECHILDREN__HPP
#define INHERITANCECLASSECHILDREN__HPP

#include "InheritanceClasseImplement.hpp"
#include "InheritanceClasseProtected.hpp"
#include "InheritancePack1.hpp"
#include "InheritanceClassePublic.hpp"

class InheritanceClasseChildren : public InheritanceClasseImplement, protected InheritanceClasseProtected, private InheritancePack1::InheritanceClassePrivate, public InheritanceClassePublic {
};

#endif
