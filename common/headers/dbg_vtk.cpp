#include "stdafx.h"
#include <string>
#include <sstream> //std::stringstream should be replaced by std::ostringstream...
#ifdef WIN32
  #include <windows.h> //for OutputDebugStringA(), which is available only for Visual Studio
#endif

#include <vtkFieldData.h>
#include <vtkCellData.h>

#include "../NfdcDataModel/Model.h"

//
//
//  ###################################################################################################
//  #                                                                                                 #
//  #                  M E T H O D S    T O    D E B U G    G E O M E T R Y                           #
//  #                                                                                                 #
//  ###################################################################################################
//
//
//
//
//  How to use those methods:
//  =================================
//      1) Put Breakpoint in code where do You want to investigate state of some geometry object.
//
//      2) When Breakpoint is hit, run "QuickWatch..." window and type there one of dbg_Print() e.g.
//            "dbg_Print(pData)" and hit Enter. (I assume that "pData" is "vtkPolyData * pData")
//            If "identifier "dbg_Print" is undefined" is displayed in "Value" column it means that
//            this file is not added to module (dll) that You are debugging. This situation 
//            should not happen, when it happen add this file as it is done for other modules.
//
//      3) Open "Output" window in Visual Studio (VS). There You see dumped data.
//            If You don't see dumped data maybe Your module is not WIN32 - fix that or
//            dump data to file.
//
//      4) You also receive result as string in "QuickWatch..." window in "Value" column 
//                (if You are seeing 3 dots "..." in the end of returned string it means it was
//                    truncated!!)
//             
//      5) Result in VS "Output" window and "error.txt" file is repeated twice. (I don't know why,
//                  but observed this in other programs also)
//
//
//    NOT Visual Studio or dumping to file
//    ------------------------------------------
//        If You are not using Visual Studio or You want to dump data to file, do following:
//            - right mouse click on AppBase module, select "Properties" next "Debugging" tab,
//                  in "Command arguments" row add ">console.txt 2>error.txt". This will redirect
//                  std::cout to "console.txt" and std::cerr to "error.txt".
//                  Those files will be created on Your HDD drive in 
//                  "$\lakota\target\vc14\Win64\dry\appbase\" directory, when Lakota (Appbase.exe)
//                  is started. When next time You run program "error.txt" will be purged.
//
//
//
//
//  List of available methods:
//  =================================
//
//      dbg_Print(const std::string & str)
//           Just print provided "str"
//
//
//      dbg_Print(vtkPolyData * pData)
//           It will print pData in table like this.
//                 cell idx | cell loc |   cell points   | body ID | surface ID 
//                --------------------------------------------------------------
//                       0  |       0  |          0,1,2  |      0  |       0  
//                       1  |       4  |          3,4,5  |      0  |       1  
//
//
//
//  READ THIS BEFORE YOU REWORK CODE HERE:
//  ========================================
//      Here are set of rules and checked things that we need to stick to, so methods are usable.
//
//      - this file is compiled in all modules that have vtk, so that during debugging when developer
//            move up and down on the call stack he can use methods from this common .cpp file.
//            Probably this is not the best solution, but I couldn't find better one.
//            When I tried to have this file compiled in one module, makes me impossible to use
//            debug methods from another module.
//
//      - return always std::string - it is visible in "QuickWatch..." window directy,
//            so developer is seeing result and can copy it to clipboard.
//
//      - methods here are not finished and if You see something wrong or missing,
//            please correct or supplement it. Just do it!!
//
//      - qDebug() << "Printed in the console";  Qt not available everywhere, so we can't use it.
//



//Goal of this method is to print dbg_ messages to std::cout.
//To dump std::cout to file call program with additional arguments like "Appbase.exe >console.txt"
std::string dbg_Print_to_cout(const std::string & str)
{
  std::cout << str << std::endl;  //There have to std::endl in the end - we want to see results imidiatelly not after we close program.

  return str;
}


//Goal of this method is to write dbg_ messages to log file.
std::string dbg_Print_to_clog(const std::string & str)
{
    //Method need to be written.
    //Should we use clog or other?
    return str;
}


//Goal of this method is to print dbg_ messages to std::cerr.
//To dump std::cerr to file call program with additional arguments like "Appbase.exe 2>error.txt"
std::string dbg_Print_to_cerr(const std::string & str)
{
    std::cerr << str << std::endl;  //There have to std::endl in the end - we want to see results imidiatelly not after we close program.

    return str;
}


//Goal of this method is to print dbg_ messages to Output window in debugger (e.g. Output in Visual Studio)
std::string dbg_Print(const std::string & str)
{
#ifdef WIN32
  OutputDebugStringA((std::string("\n\nDBG: ") + str + std::string("\n\n\n")).c_str());
  return str;
#else
  return dbg_Print_to_cerr(str); //I don't know how to dump data to Output window in debuger right now so I am redirecting to std::cerr.
#endif
}

std::string dbg_Print(const std::string * pStr)
{
  if (!pStr)
    return dbg_Print(std::string("nullptr passed instead of std::string"));

  return dbg_Print(*pStr);
}

//This method was never tested
std::string dbg_Print(std::ostream & os)
{
  //std::ostream os(std::cout.rdbuf()); //this is how to create std::ostream that will write to std::cout

  std::stringstream ss;
  ss << os.rdbuf();
  std::string str = ss.str();

  return dbg_Print(str);
}






// see description in top of header.
std::string dbg_Print(vtkPolyData * pData)
{
  const char* bodyIdsName = "BodyIds";
  const char* surfaceIdsName = "SurfaceIds";

  if (!pData)
    return "Provided pointer to vtkPolyData is nullptr.";

  if (pData->GetNumberOfPolys() != pData->GetCellData()->GetNumberOfTuples())
    return "Provided vtkPolyData is broken! There is different number of Polys from Tuples.";

  vtkIdTypeArray * pBodyIds = vtkIdTypeArray::SafeDownCast(pData->GetCellData()->GetArray(bodyIdsName));
  if (!pBodyIds)
    return std::string("Provided vtkPolyData is broken! It does not contain '") + std::string(bodyIdsName) + std::string("' array.");

  vtkIdTypeArray * pSurfaceIds = vtkIdTypeArray::SafeDownCast(pData->GetCellData()->GetArray(surfaceIdsName));
  if (!pSurfaceIds)
    return std::string("Provided vtkPolyData is broken! It does not contain '") + std::string(surfaceIdsName) + std::string("' array.");

  vtkCellArray * pCells = pData->GetPolys();
  if (!pCells)
    return "Provided vtkPolyData is broken! It does not contain Cell array.";

  if ((pCells->GetNumberOfCells() != pBodyIds->GetNumberOfValues()) || (pCells->GetNumberOfCells() != pSurfaceIds->GetNumberOfValues())) //I do not know if this condition is correct
    return std::string("Provided vtkPolyData is broken! It does not different number of cells from '") + std::string(bodyIdsName) + std::string("' from '") + std::string(surfaceIdsName) + std::string("' arrays.");

  vtkIdType nbr = pCells->GetNumberOfCells();

  std::stringstream stream;
  stream << std::endl;
  stream << " cell idx | cell loc |   cell points   | body ID | surface ID " << std::endl;
  stream << "--------------------------------------------------------------" << std::endl;
  pCells->InitTraversal();
  vtkIdType cellLoc = 0;
  vtkIdType cellNpts;
  for (vtkIdType ii = 0; ii < nbr; ii++)
  {
    { //iterate through cells
      vtkIdType* pts = nullptr;
      if (!pCells->GetNextCell(cellNpts, pts))
        return std::string("Provided vtkPolyData is broken! Can not iterate through cells.");

      auto ptsl = vtkSmartPointer<vtkIdList>::New();
      pCells->GetCell(cellLoc, ptsl);
      stream << "  " << std::setw(6) << ii << "  |  " << std::setw(6) << cellLoc << "  |  "; //dump cell index (idx) and cell location (loc)
      std::stringstream ptsStream;
      for (vtkIdType kk = 0; kk < ptsl->GetNumberOfIds(); kk++)
        ptsStream << ((kk == 0) ? "" : ",") << ptsl->GetId(kk); //dump points

      stream << std::setw(13) << ptsStream.str();
      cellLoc = pCells->GetTraversalLocation();
    }

    stream << "  |  " << std::setw(5) << pBodyIds->GetValue(ii) << "  |  " << std::setw(6) << pSurfaceIds->GetValue(ii) << "  " << std::endl;
  }
  stream << std::endl;

  vtkIdType* pts = nullptr;
  if (pCells->GetNextCell(cellNpts, pts))
    return std::string("Provided vtkPolyData is broken! There is more cells than other data.");

  return dbg_Print(stream.str()); //convert to string
}


// see description in top of header.
std::string dbg_Print(vtkIdTypeArray * pData)
{
    std::stringstream stream;
    stream << std::endl;
    stream << "--------------------------------------------------------------" << std::endl;

    if (pData->GetNumberOfComponents() == 1)
    {
        for (int i = 0; i < pData->GetNumberOfValues(); i++)
        {
            stream << i << "  |  " << pData->GetValue(i) << std::endl;
        }
    }
    if (pData->GetNumberOfComponents() == 2)
    {
        for (int i = 0; i < pData->GetNumberOfTuples(); i++)
        {
            auto tuple = pData->GetTuple2(i);
            stream << i << "  |  " << tuple[0] << " - " << tuple[1] << std::endl;
        }
    }

    return dbg_Print(stream.str()); //convert to string
}

std::string dbg_Print(SIM::Geometry * geometry)
{
    const char* edgeIdsName = "EdgeIds";

    std::stringstream stream;
    stream << std::endl;

    for (auto body : geometry->GetBodies())
    {
        stream << "--------------------------------------------------------------" << std::endl;
        stream << ">>> BODY " << body.second->GetItemId() << std::endl;

        auto rawgeometry = body.second->GetRawGeometry();

        auto fd = rawgeometry->GetFieldData();
        if (!fd)
            continue;

        stream << "---- edges " << std::endl;

        auto edgeIds = vtkIdTypeArray::SafeDownCast(fd->GetArray(edgeIdsName));
        for (int i = 0; i < edgeIds->GetNumberOfTuples(); i++)
        {
            double* t = edgeIds->GetTuple(i);
            stream << i << "  |  ";
            for (int c = 0; c < edgeIds->GetNumberOfComponents(); c++)
                stream << " " << t[c];
            stream << std::endl;
        }
    }

    stream << std::endl << std::endl;
    stream << ">>> STRUCTURE" << std::endl;
    for (auto body : geometry->GetBodies())
    {
        stream << "--------------------------------------------------------------" << std::endl;
        stream << ">>> BODY " << body.second->GetItemId() << std::endl;

        stream << "---- surfaces" << std::endl;
        for (auto srf : body.second->GetSurfaces())
        {
            stream << srf.second->GetItemId() << std::endl;
        }
        stream << "---- edges" << std::endl;
        for (auto edge : body.second->GetEdges())
        {
            stream << edge.second->GetItemId() << std::endl;
        }
        stream << "---- nodes" << std::endl;
        for (auto node : body.second->GetNodes())
        {
            stream << node.second->GetItemId() << std::endl;
        }
    }

    return dbg_Print(stream.str()); //convert to string
}

std::string dbg_Print(std::shared_ptr<SIM::Geometry> geometry)
{
    return dbg_Print(geometry.get());
}

std::string dbg_Print(SIM::IdsMap* map)
{
    std::stringstream stream;
    stream << std::endl;

    stream << "left" << std::endl;
    for (auto item : map->left)
    {
        stream << item.first << " | " << item.second << std::endl;
    }

    stream << "right" << std::endl;
    for (auto item : map->right)
    {
        stream << item.first << " | " << item.second << std::endl;
    }

    return dbg_Print(stream.str()); //convert to string
}