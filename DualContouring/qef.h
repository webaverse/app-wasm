#ifndef QEF_H
#define QEF_H
#ifndef NO_OSTREAM
#include <iostream>
#endif

#include "svd.h"
namespace svd
{
    class QefData
    {
    public:
        float ata_00, ata_01, ata_02, ata_11, ata_12, ata_22;
        float atb_x, atb_y, atb_z;
        float btb;
        float massPoint_x, massPoint_y, massPoint_z;
        int numPoints;

        QefData();

        QefData(const float ata_00, const float ata_01,
                const float ata_02, const float ata_11, const float ata_12,
                const float ata_22, const float atb_x, const float atb_y,
                const float atb_z, const float btb, const float massPoint_x,
                const float massPoint_y, const float massPoint_z,
                const int numPoints) ;

        void add(const QefData &rhs) ;

        void clear() ;

        void set(const float ata_00, const float ata_01,
                 const float ata_02, const float ata_11, const float ata_12,
                 const float ata_22, const float atb_x, const float atb_y,
                 const float atb_z, const float btb, const float massPoint_x,
                 const float massPoint_y, const float massPoint_z,
                 const int numPoints) ;

        void set(const QefData &rhs);

        QefData(const QefData &rhs);
        QefData &operator= (const QefData &rhs);
    };
#ifndef NO_OSTREAM
    std::ostream &operator<<(std::ostream &os, const QefData &d) ;
#endif
    class QefSolver
    {
    private:
        QefData data;
        SMat3 ata;
        Vec3 atb, massPoint, x;
        bool hasSolution;
    public:
        QefSolver() ;
    public:

		const Vec3& getMassPoint() const { return massPoint; }

        void add(const float px, const float py, const float pz,
                 float nx, float ny, float nz) ;
        void add(const Vec3 &p, const Vec3 &n);
        void add(const QefData &rhs) ;
		QefData getData() ;
        float getError();
        float getError(const Vec3 &pos);
        void reset();
        float solve(Vec3 &outx, const float svd_tol,
                     const int svd_sweeps, const float pinv_tol) ;
    private:
        QefSolver(const QefSolver &rhs);
        QefSolver &operator=(const QefSolver &rhs);
        void setAta();
        void setAtb() ;
    };
};
#endif