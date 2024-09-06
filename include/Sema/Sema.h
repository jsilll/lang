#ifndef LANG_SEMA_H
#define LANG_SEMA_H

#include "Frontend/ASTVisitor.h"

namespace lang {

class Sema : public ASTVisitor<Sema> {};

} // namespace lang

#endif // LANG_SEMA_H
