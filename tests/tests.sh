#!/bin/sh

mkdir result
rm -Rf result/*

../src/dia2code -t cpp association.dia -d result || exit 1
if [[ ! -a result/AssociationClass1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationPack.hpp ]] ; then
exit 1
fi ;
diff -pu AssociationClass1.hpp result/AssociationClass1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationPack.hpp result/AssociationPack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp association.dia -d result -1 || exit 1
if [[ ! -a result/AssociationClass1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationClass2.hpp ]] ; then
exit 1
fi ;
diff -pu AssociationClass1-1.hpp result/AssociationClass1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationClass2-1.hpp result/AssociationClass2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp association.dia -d result --buildtree || exit 1
if [[ ! -a result/AssociationClass1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationPack/AssociationClass2.hpp ]] ; then
exit 1
fi ;
diff -pu AssociationClass1-b.hpp result/AssociationClass1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationClass2-b.hpp result/AssociationPack/AssociationClass2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp package.dia -d result || exit 1
if [[ ! -a result/package.hpp ]] ; then
exit 1
fi ;
diff -pu package.hpp result/package.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp package.dia -d result -1 || exit 1
if [[ ! -a result/PackageClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/PackageClasse2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/PackageClasse3.hpp ]] ; then
exit 1
fi ;
diff -pu PackageClasse1.hpp result/PackageClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse2.hpp result/PackageClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse3.hpp result/PackageClasse3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp package.dia -d result --buildtree || exit 1
if [[ ! -a result/package/PackageClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/package/pack2/PackageClasse2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/package/pack2/pack3/PackageClasse3.hpp ]] ; then
exit 1
fi ;
diff -pu PackageClasse1-b.hpp result/package/PackageClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse2-b.hpp result/package/pack2/PackageClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse3-b.hpp result/package/pack2/pack3/PackageClasse3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp enum.dia -d result || exit 1
if [[ ! -a result/enumeration.hpp ]] ; then
exit 1
fi ;
diff -pu enumeration.hpp result/enumeration.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp operations.dia -d result || exit 1
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu operations.hpp result/operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm result/operations.hpp
../src/dia2code -t cpp operations.dia -d result -l COPYING || exit 1
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu operations_COPYING.hpp result/operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp attributes.dia -d result || exit 1
if [[ ! -a result/attributes.hpp ]] ; then
exit 1
fi ;
diff -pu attributes.hpp result/attributes.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp dependances.dia -d result || exit 1
if [[ ! -a result/dependances.hpp ]] ; then
exit 1
fi ;
diff -pu dependances.hpp result/dependances.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp dependances_pack.dia -d result || exit 1
if [[ ! -a result/DepClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack.hpp ]] ; then
exit 1
fi ;
diff -pu DepPack.hpp result/DepPack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1.hpp result/DepClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2.hpp result/DepClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3.hpp result/DepClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4.hpp result/DepClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp dependances_pack.dia -d result -1 || exit 1
if [[ ! -a result/DepClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClasse2.hpp ]] ; then
exit 1
fi ;
diff -pu DepClasse1.hpp result/DepClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClasse2.hpp result/DepClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1-1.hpp result/DepClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2-1.hpp result/DepClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3-1.hpp result/DepClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4-1.hpp result/DepClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp dependances_pack.dia -d result --buildtree || exit 1
if [[ ! -a result/DepClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack/DepClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack/pack2/DepClasse2.hpp ]] ; then
exit 1
fi ;
diff -pu DepClasse1-b.hpp result/DepPack/DepClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClasse2-b.hpp result/DepPack/pack2/DepClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1-b.hpp result/DepClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2-b.hpp result/DepClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3-b.hpp result/DepClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4-b.hpp result/DepClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp structure.dia -d result || exit 1
if [[ ! -a result/structure.hpp ]] ; then
exit 1
fi ;
diff -pu structure.hpp result/structure.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp inherence.dia -d result || exit 1
if [[ ! -a result/InherenceClasseChildren.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherencePack1.hpp ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren.hpp result/InherenceClasseChildren.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent.hpp result/InherenceClasseParent.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherencePack1.hpp result/InherencePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp inherence.dia -d result -1 || exit 1
if [[ ! -a result/InherenceClasseChildren.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent2.hpp ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren-1.hpp result/InherenceClasseChildren.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent-1.hpp result/InherenceClasseParent.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent2-1.hpp result/InherenceClasseParent2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp inherence.dia -d result --buildtree || exit 1
if [[ ! -a result/InherenceClasseChildren.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherencePack1/InherenceClasseParent2.hpp ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren-b.hpp result/InherenceClasseChildren.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent-b.hpp result/InherenceClasseParent.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent2-b.hpp result/InherencePack1/InherenceClasseParent2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp typedef.dia -d result || exit 1
if [[ ! -a result/TypeDef.hpp ]] ; then
exit 1
fi ;
diff -pu TypeDef.hpp result/TypeDef.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp getset.dia -d result || exit 1
if [[ ! -a result/GetSet.hpp ]] ; then
exit 1
fi ;
diff -pu GetSet.hpp result/GetSet.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp constance.dia -d result || exit 1
if [[ ! -a result/constance.hpp ]] ; then
exit 1
fi ;
diff -pu constance.hpp result/constance.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp template.dia -d result || exit 1
if [[ ! -a result/Template.hpp ]] ; then
exit 1
fi ;
diff -pu Template.hpp result/Template.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;


rm -Rf result/*

../src/dia2code -t cpp11 association.dia -d result || exit 1
if [[ ! -a result/AssociationClass1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationPack.hpp ]] ; then
exit 1
fi ;
diff -pu AssociationClass1.hpp result/AssociationClass1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationPack.hpp result/AssociationPack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 association.dia -d result -1 || exit 1
if [[ ! -a result/AssociationClass1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationClass2.hpp ]] ; then
exit 1
fi ;
diff -pu AssociationClass1-1.hpp result/AssociationClass1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationClass2-1.hpp result/AssociationClass2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 association.dia -d result --buildtree || exit 1
if [[ ! -a result/AssociationClass1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationPack/AssociationClass2.hpp ]] ; then
exit 1
fi ;
diff -pu AssociationClass1-b.hpp result/AssociationClass1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationClass2-b.hpp result/AssociationPack/AssociationClass2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 package.dia -d result || exit 1
if [[ ! -a result/package.hpp ]] ; then
exit 1
fi ;
diff -pu package.hpp result/package.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 package.dia -d result -1 || exit 1
if [[ ! -a result/PackageClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/PackageClasse2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/PackageClasse3.hpp ]] ; then
exit 1
fi ;
diff -pu PackageClasse1.hpp result/PackageClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse2.hpp result/PackageClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse3.hpp result/PackageClasse3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 package.dia -d result --buildtree || exit 1
if [[ ! -a result/package/PackageClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/package/pack2/PackageClasse2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/package/pack2/pack3/PackageClasse3.hpp ]] ; then
exit 1
fi ;
diff -pu PackageClasse1-b.hpp result/package/PackageClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse2-b.hpp result/package/pack2/PackageClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse3-b.hpp result/package/pack2/pack3/PackageClasse3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 enum.dia -d result || exit 1
if [[ ! -a result/enumeration.hpp ]] ; then
exit 1
fi ;
diff -pu enumeration-11.hpp result/enumeration.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 operations.dia -d result || exit 1
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu operations-11.hpp result/operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm result/operations.hpp
../src/dia2code -t cpp11 operations.dia -d result -l COPYING || exit 1
if [[ ! -a result/operations.hpp ]] ; then
exit 1
fi ;
diff -pu operations_COPYING-11.hpp result/operations.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 attributes.dia -d result || exit 1
if [[ ! -a result/attributes.hpp ]] ; then
exit 1
fi ;
diff -pu attributes.hpp result/attributes.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 dependances.dia -d result || exit 1
if [[ ! -a result/dependances.hpp ]] ; then
exit 1
fi ;
diff -pu dependances.hpp result/dependances.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 dependances_pack.dia -d result || exit 1
if [[ ! -a result/DepClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack.hpp ]] ; then
exit 1
fi ;
diff -pu DepPack.hpp result/DepPack.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1.hpp result/DepClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2.hpp result/DepClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3.hpp result/DepClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4.hpp result/DepClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 dependances_pack.dia -d result -1 || exit 1
if [[ ! -a result/DepClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClasse2.hpp ]] ; then
exit 1
fi ;
diff -pu DepClasse1.hpp result/DepClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClasse2.hpp result/DepClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1-1.hpp result/DepClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2-1.hpp result/DepClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3-1.hpp result/DepClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4-1.hpp result/DepClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 dependances_pack.dia -d result --buildtree || exit 1
if [[ ! -a result/DepClassePack1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack/DepClasse1.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack/pack2/DepClasse2.hpp ]] ; then
exit 1
fi ;
diff -pu DepClasse1-b.hpp result/DepPack/DepClasse1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClasse2-b.hpp result/DepPack/pack2/DepClasse2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1-b.hpp result/DepClassePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2-b.hpp result/DepClassePack2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3-b.hpp result/DepClassePack3.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4-b.hpp result/DepClassePack4.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 structure.dia -d result || exit 1
if [[ ! -a result/structure.hpp ]] ; then
exit 1
fi ;
diff -pu structure.hpp result/structure.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 inherence.dia -d result || exit 1
if [[ ! -a result/InherenceClasseChildren.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherencePack1.hpp ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren.hpp result/InherenceClasseChildren.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent.hpp result/InherenceClasseParent.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherencePack1.hpp result/InherencePack1.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 inherence.dia -d result -1 || exit 1
if [[ ! -a result/InherenceClasseChildren.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent2.hpp ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren-1.hpp result/InherenceClasseChildren.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent-1.hpp result/InherenceClasseParent.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent2-1.hpp result/InherenceClasseParent2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t cpp11 inherence.dia -d result --buildtree || exit 1
if [[ ! -a result/InherenceClasseChildren.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.hpp ]] ; then
exit 1
fi ;
if [[ ! -a result/InherencePack1/InherenceClasseParent2.hpp ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren-b.hpp result/InherenceClasseChildren.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent-b.hpp result/InherenceClasseParent.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent2-b.hpp result/InherencePack1/InherenceClasseParent2.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 typedef.dia -d result || exit 1
if [[ ! -a result/TypeDef.hpp ]] ; then
exit 1
fi ;
diff -pu TypeDef.hpp result/TypeDef.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 getset.dia -d result || exit 1
if [[ ! -a result/GetSet.hpp ]] ; then
exit 1
fi ;
diff -pu GetSet-11.hpp result/GetSet.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 constance.dia -d result || exit 1
if [[ ! -a result/constance.hpp ]] ; then
exit 1
fi ;
diff -pu constance-11.hpp result/constance.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t cpp11 template.dia -d result || exit 1
if [[ ! -a result/Template.hpp ]] ; then
exit 1
fi ;
diff -pu Template.hpp result/Template.hpp > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;


rm -Rf result/*

../src/dia2code -t java association.dia -d result || exit 1
if [[ ! -a result/AssociationClass1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationClass2.java ]] ; then
exit 1
fi ;
diff -pu AssociationClass1.java result/AssociationClass1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationClass2.java result/AssociationClass2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java association.dia -d result --buildtree || exit 1
if [[ ! -a result/AssociationClass1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/AssociationPack/AssociationClass2.java ]] ; then
exit 1
fi ;
diff -pu AssociationClass1-b.java result/AssociationClass1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu AssociationClass2-b.java result/AssociationPack/AssociationClass2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java package.dia -d result || exit 1
if [[ ! -a result/PackageClasse1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/PackageClasse2.java ]] ; then
exit 1
fi ;
if [[ ! -a result/PackageClasse3.java ]] ; then
exit 1
fi ;
diff -pu PackageClasse1.java result/PackageClasse1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse2.java result/PackageClasse2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse3.java result/PackageClasse3.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java package.dia -d result --buildtree || exit 1
if [[ ! -a result/package/PackageClasse1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/package/pack2/PackageClasse2.java ]] ; then
exit 1
fi ;
if [[ ! -a result/package/pack2/pack3/PackageClasse3.java ]] ; then
exit 1
fi ;
diff -pu PackageClasse1-b.java result/package/PackageClasse1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse2-b.java result/package/pack2/PackageClasse2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu PackageClasse3-b.java result/package/pack2/pack3/PackageClasse3.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java enum.dia -d result || exit 1
if [[ ! -a result/enumeration.java ]] ; then
exit 1
fi ;
diff -pu enumeration.java result/enumeration.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t java operations.dia -d result || exit 1
if [[ ! -a result/operations.java ]] ; then
exit 1
fi ;
diff -pu operations.java result/operations.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm result/operations.java
../src/dia2code -t java operations.dia -d result -l COPYING || exit 1
if [[ ! -a result/operations.java ]] ; then
exit 1
fi ;
diff -pu operations_COPYING.java result/operations.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t java attributes.dia -d result || exit 1
if [[ ! -a result/attributes.java ]] ; then
exit 1
fi ;
diff -pu attributes.java result/attributes.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t java dependances.dia -d result || exit 1
if [[ ! -a result/dependances.java ]] ; then
exit 1
fi ;
if [[ ! -a result/Classe1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/Classe2.java ]] ; then
exit 1
fi ;
diff -pu dependances.java result/dependances.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu Classe1.java result/Classe1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu Classe2.java result/Classe2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java dependances_pack.dia -d result || exit 1
if [[ ! -a result/DepClassePack1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClasse1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClasse2.java ]] ; then
exit 1
fi ;
diff -pu DepClasse1.java result/DepClasse1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClasse2.java result/DepClasse2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1.java result/DepClassePack1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2.java result/DepClassePack2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3.java result/DepClassePack3.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4.java result/DepClassePack4.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java dependances_pack.dia -d result --buildtree || exit 1
if [[ ! -a result/DepClassePack1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack2.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack3.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepClassePack4.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack/DepClasse1.java ]] ; then
exit 1
fi ;
if [[ ! -a result/DepPack/pack2/DepClasse2.java ]] ; then
exit 1
fi ;
diff -pu DepClasse1-b.java result/DepPack/DepClasse1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClasse2-b.java result/DepPack/pack2/DepClasse2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack1-b.java result/DepClassePack1.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack2-b.java result/DepClassePack2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack3-b.java result/DepClassePack3.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu DepClassePack4-b.java result/DepClassePack4.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t java structure.dia -d result || exit 1
if [[ ! -a result/structure.java ]] ; then
exit 1
fi ;
diff -pu structure.java result/structure.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java inherence.dia -d result || exit 1
if [[ ! -a result/InherenceClasseChildren.java ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.java ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent2.java ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren.java result/InherenceClasseChildren.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent.java result/InherenceClasseParent.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent2.java result/InherenceClasseParent2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

rm -Rf result/*
../src/dia2code -t java inherence.dia -d result --buildtree || exit 1
if [[ ! -a result/InherenceClasseChildren.java ]] ; then
exit 1
fi ;
if [[ ! -a result/InherenceClasseParent.java ]] ; then
exit 1
fi ;
if [[ ! -a result/InherencePack1/InherenceClasseParent2.java ]] ; then
exit 1
fi ;
diff -pu InherenceClasseChildren-b.java result/InherenceClasseChildren.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent-b.java result/InherenceClasseParent.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;
diff -pu InherenceClasseParent2-b.java result/InherencePack1/InherenceClasseParent2.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

../src/dia2code -t java typedef.dia -d result || exit 1
if [[ ! -a result/TypeDef.java ]] ; then
exit 1
fi ;
diff -pu TypeDef.java result/TypeDef.java > result.txt
if [[ -s result.txt ]] ; then
exit 1
fi ;

#../src/dia2code -t java getset.dia -d result || exit 1
#if [[ ! -a result/GetSet.java ]] ; then
#exit 1
#fi ;
#diff -pu GetSet-11.java result/GetSet.java > result.txt
#if [[ -s result.txt ]] ; then
#exit 1
#fi ;
#
#../src/dia2code -t java constance.dia -d result || exit 1
#if [[ ! -a result/constance.java ]] ; then
#exit 1
#fi ;
#diff -pu constance-11.java result/constance.java > result.txt
#if [[ -s result.txt ]] ; then
#exit 1
#fi ;
#
#../src/dia2code -t java template.dia -d result || exit 1
#if [[ ! -a result/Template.java ]] ; then
#exit 1
#fi ;
#diff -pu Template.java result/Template.java > result.txt
#if [[ -s result.txt ]] ; then
#exit 1
#fi ;
#
#
exit 0
