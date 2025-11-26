#pragma once
    #include <iostream>
    #include "../Core.hpp"

extern Core _core;
//Product class
namespace MiniBuilder {
    class EntityBuilder {
        private:
            Entity _entity;
        public:
            EntityBuilder(Entity entity) {
                this->_entity = entity;
            }
            template<typename...args>
            void BuildEntity(args&&... comps) {
                (_core.AddComponent(_entity, std::forward<args>(comps)), ...);
            };
    };

    class RegisterComponentBuilder {
        public:
            template<typename... TComponents>
            void RegisterComponents() {
                (_core.RegisterComponent<TComponents>(), ...);
            }
    };

    class SystemBuilder {
        private:
            Signature _signature;
        public:
            SystemBuilder(Signature signature): _signature(signature){}

            template<typename Sys, typename...TComponents>
            void BuildSignature() {
                (_signature.set(_core.GetComponentType<TComponents>(), true), ...);

                _core.SetSystemSignature<Sys>(_signature);
            }
    };
}