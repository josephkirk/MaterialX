//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#include <MaterialXTest/Catch/catch.hpp>

#include <MaterialXCore/Document.h>

namespace mx = MaterialX;

TEST_CASE("Look", "[look]")
{
    mx::DocumentPtr doc = mx::createDocument();

    // Create a material and look.
    mx::MaterialPtr material = doc->addMaterial();
    mx::ShaderRefPtr shaderRef = material->addShaderRef();
    mx::LookPtr look = doc->addLook();
    REQUIRE(doc->getMaterials().size() == 1);
    REQUIRE(doc->getLooks().size() == 1);

    // Bind the material to a geometry string.
    mx::MaterialAssignPtr matAssign1 = look->addMaterialAssign("matAssign1", material->getName());
    matAssign1->setGeom("/robot1");
    REQUIRE(matAssign1->getReferencedMaterial() == material);
    REQUIRE(material->getBoundGeomStrings()[0] == "/robot1");

    // Bind the material to a geometric collection.
    mx::MaterialAssignPtr matAssign2 = look->addMaterialAssign("matAssign2", material->getName());
    mx::CollectionPtr collection = doc->addCollection();
    mx::CollectionAddPtr collectionAdd = collection->addCollectionAdd();
    collectionAdd->setGeom("/robot2");
    mx::CollectionRemovePtr collectionRemove = collection->addCollectionRemove();
    collectionRemove->setGeom("/robot2/left_arm");
    matAssign2->setCollection(collection);
    REQUIRE(material->getBoundGeomCollections()[0] == collection);

    // Create a property assignment.
    mx::PropertyAssignPtr propertyAssign = look->addPropertyAssign("twosided");
    propertyAssign->setGeom("/robot1");
    propertyAssign->setValue(true);
    REQUIRE(propertyAssign->getGeom() == "/robot1");
    REQUIRE(propertyAssign->getValue()->isA<bool>());
    REQUIRE(propertyAssign->getValue()->asA<bool>() == true);

    // Create a property set assignment.
    mx::PropertySetPtr propertySet = doc->addPropertySet();
    propertySet->setPropertyValue("matte", false);
    REQUIRE(propertySet->getPropertyValue("matte")->isA<bool>());
    REQUIRE(propertySet->getPropertyValue("matte")->asA<bool>() == false);
    mx::PropertySetAssignPtr propertySetAssign = look->addPropertySetAssign(propertySet->getName());
    propertySetAssign->setGeom("/robot1");
    REQUIRE(propertySetAssign->getGeom() == "/robot1");

    // Create a visibility element.
    mx::VisibilityPtr visibility = look->addVisibility();
    REQUIRE(visibility->getVisible() == false);
    visibility->setVisible(true);
    REQUIRE(visibility->getVisible() == true);
    visibility->setGeom("/robot2");
    REQUIRE(visibility->getGeom() == "/robot2");
    visibility->setCollection(collection);
    REQUIRE(visibility->getCollection() == collection);

    // Create an inherited look.
    mx::LookPtr look2 = doc->addLook();
    look2->setInheritsFrom(look);
    REQUIRE(look2->getActiveMaterialAssigns().size() == 2);
    REQUIRE(look2->getActivePropertySetAssigns().size() == 1);
    REQUIRE(look2->getActiveVisibilities().size() == 1);

    // Create and detect an inheritance cycle.
    look->setInheritsFrom(look2);
    REQUIRE(!doc->validate());
    look->setInheritsFrom(nullptr);
    REQUIRE(doc->validate());

    // Disconnect the inherited look.
    look2->setInheritsFrom(nullptr);
    REQUIRE(look2->getActiveMaterialAssigns().empty());
    REQUIRE(look2->getActivePropertySetAssigns().empty());
    REQUIRE(look2->getActiveVisibilities().empty());
}
