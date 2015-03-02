<?php
require_once 'InheritanceClasseImplement.php';
require_once 'InheritanceClasseProtected.php';
require_once 'InheritancePack1/InheritanceClassePrivate.php';
require_once 'InheritanceClassePublic.php';

class InheritanceClasseChildren extends InheritanceClasseImplement extends InheritanceClasseProtected extends \InheritancePack1\InheritanceClassePrivate extends InheritanceClassePublic {
}
?>
