#ifndef COMMON_H
#define COMMON_H
#if defined(QT_BUILD_COMPONENTS_LIB)
# define Q_COMPONENTS_EXPORT Q_DECL_EXPORT
#else
# define Q_COMPONENTS_EXPORT Q_DECL_IMPORT
#endif

#endif // COMMON_H
