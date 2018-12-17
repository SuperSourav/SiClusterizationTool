/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : NnPixelClusterSplitProbTool.cxx
/// Package : SiClusterizationTool 
/// Author  : Giacinto Piacquadio (PH-ADE-ID)
/// Created : January 2011
///
/// DESCRIPTION: Compute cluster splitting probabilities 
/// (for splitting a cluster into  2 .... N subclusters)
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/NnPixelClusterSplitProbTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "VxVertex/RecVertex.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

//#include <time.h>

//trigtimer
//#include "TrigSteering/TrigSteer.h"
#include "TrigTimeAlgs/TrigTimer.h"

namespace InDet
{
  

  NnPixelClusterSplitProbTool::NnPixelClusterSplitProbTool(const std::string& t, const std::string& n, const IInterface*  p)
          :AthAlgTool(t,n,p),
           m_NnClusterizationFactory("InDet::NnClusterizationFactory/NnClusterizationFactory"),
           m_iBeamCondSvc("BeamCondSvc",n),
           m_useBeamSpotInfo(true),
	   m_timer(0),
	   m_timerSvc("TrigTimerSvc/TrigTimerSvc", n)
	   //m_numNNTimer(nullptr)
  {

    m_priorMultiplicityContent.push_back(2793337);
    m_priorMultiplicityContent.push_back(82056);
    m_priorMultiplicityContent.push_back(19944);


    declareInterface<IPixelClusterSplitProbTool>(this);

    declareProperty("NnClusterizationFactory",m_NnClusterizationFactory);
    declareProperty("BeamCondSv",m_iBeamCondSvc);
    declareProperty("PriorMultiplicityContent",m_priorMultiplicityContent);
    declareProperty("useBeamSpotInfo",m_useBeamSpotInfo);


  }
  

  StatusCode NnPixelClusterSplitProbTool::initialize()
  {
    
 
    if (m_NnClusterizationFactory.retrieve().isFailure())
    {
      ATH_MSG_ERROR(" Unable to retrieve "<< m_NnClusterizationFactory );
      return StatusCode::FAILURE;
    }

    if (m_iBeamCondSvc.retrieve().isFailure())
    {
      ATH_MSG_ERROR( "Could not find BeamCondSvc." );
      return StatusCode::FAILURE;
    }

    // trigtimer
    //if (m_timerSvc.retrieve().isFailure())
    //{
    //  ATH_MSG_ERROR(" Unable to retrieve "<< m_timerSvc );
    //  return StatusCode::FAILURE;
    //}
    //else
    //{
    /*m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
    if(!m_parentAlg) {
      ATH_MSG_ERROR(" Unable to cast the parent algorithm to HLT::TrigSteer!");
      return StatusCode::FAILURE;
    }*/
    CHECK(m_timerSvc.retrieve());
    ATH_MSG_INFO("Retrieved TrigTimerSvc");
    m_timer = m_timerSvc->addItem("numNettimer");
    ATH_MSG_INFO("TriTimer name: " << m_timer->name() );
    //}
    //

    ATH_MSG_INFO(" Cluster split prob tool initialized successfully "<< m_NnClusterizationFactory );
    return StatusCode::SUCCESS;
  }

  
  InDet::PixelClusterSplitProb NnPixelClusterSplitProbTool::splitProbability(const InDet::PixelCluster& origCluster ) const
  {
    
    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());
    Amg::Vector3D beamSpotPosition(beamposition.position()[0],
                                beamposition.position()[1],
                                beamposition.position()[2]);

    if (!m_useBeamSpotInfo) beamSpotPosition=Amg::Vector3D(0,0,0);

    std::vector<double> vectorOfProbs=m_NnClusterizationFactory->estimateNumberOfParticles(origCluster,beamSpotPosition);
    std::cout << ">>>>>>>>>>>>>>>>>Hello from No tracks, vectorsize: "  << std::endl;

    ATH_MSG_VERBOSE(" Got splitProbability, size of vector: " << vectorOfProbs.size() );

    if (vectorOfProbs.size()==0)
    {
      std::vector<double> vectorOfSplitProbs;
      vectorOfSplitProbs.push_back(-100);
      PixelClusterSplitProb  clusterSplitProb(vectorOfSplitProbs);
      ATH_MSG_VERBOSE(" Returning single split prob equal to -100 " );
      return clusterSplitProb;
    }
     

    ATH_MSG_VERBOSE( 
        " P(1): " << vectorOfProbs[0] << 
        " P(2): " << vectorOfProbs[1] << 
        " P(>=3): " << vectorOfProbs[2] );


    return compileSplitProbability(vectorOfProbs); 
  }

  InDet::PixelClusterSplitProb NnPixelClusterSplitProbTool::splitProbability(const InDet::PixelCluster& origCluster, const Trk::TrackParameters& trackParameters ) const
  {
    
    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx());
    Amg::Vector3D beamSpotPosition(beamposition.position()[0],
                                beamposition.position()[1],
                                beamposition.position()[2]);

    if (!m_useBeamSpotInfo) beamSpotPosition=Amg::Vector3D(0,0,0);

    //clock_t t2 = clock();
    //int repeats = 10000;
    std::vector<double> vectorOfProbs;
    //for (int s=0; s<repeats; s++){
    //declare trigtimer smart pointer
    //bool active = true;
    //TrigTimer* numNNTimer;
    //std::string itemname="numNNTimerA";
    //numNNTimer = new TrigTimer(itemname, active); //, true);
    //std::unique_ptr<TrigTimer> numNNTimer;
    //numNNTimer = std::make_unique<TrigTimer>("numNNTimer");
    //starting the trigtimer
    std::cout  << " ****** cluster starts **************"  << m_timer->isActive() << std::endl;
    m_timer->start();
      vectorOfProbs=m_NnClusterizationFactory->estimateNumberOfParticles(origCluster, trackParameters.associatedSurface(), trackParameters);
    //stopping the trigtimer
    m_timer->stop();
    //delete numNNTimer;
    //}
    //t2 = clock() - t2;
    //std::cout << "~~~~~~~~~~~~~~~~~~~~~~~CLOCK~~~~~~~~~~~> numNN call (w/) trk info: " << ((float)t2 * 1000000)/(repeats*CLOCKS_PER_SEC) << " micro-secs" << "total time: " << ((float)t2/CLOCKS_PER_SEC) << "sec" << std::endl;
    //printing out the time
    std::cout << m_timer->elapsed() << " ms -> NumNN call (w/ trackinfo) *******************TRIGTIMER vectorsize: " << vectorOfProbs.size() << std::endl;

    ATH_MSG_VERBOSE(" Got splitProbability, size of vector: " << vectorOfProbs.size() );

    if (vectorOfProbs.size()==0)
    {
      std::vector<double> vectorOfSplitProbs;
      vectorOfSplitProbs.push_back(-100);
      PixelClusterSplitProb  clusterSplitProb(vectorOfSplitProbs);
      ATH_MSG_VERBOSE(" Returning single split prob equal to -100 " );
      return clusterSplitProb;
    }
     

    ATH_MSG_VERBOSE( 
        " P(1): " << vectorOfProbs[0] << 
        " P(2): " << vectorOfProbs[1] << 
        " P(>=3): " << vectorOfProbs[2] );


    return compileSplitProbability(vectorOfProbs); 
  }



  InDet::PixelClusterSplitProb NnPixelClusterSplitProbTool::compileSplitProbability(std::vector<double>& vectorOfProbs ) const
  {


    double sum=0;

    std::vector<double>::iterator begin=vectorOfProbs.begin();
    std::vector<double>::iterator end=vectorOfProbs.end();

    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      sum+=*iter;
    }

    
    ATH_MSG_VERBOSE(" Sum of cluster probabilities is: "<<sum);

    std::vector<double> vectorOfSplitProbs;

    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      (*iter)/=sum;
    }

    if (m_priorMultiplicityContent.size()<vectorOfProbs.size())
    {
      ATH_MSG_ERROR("Prior compatibilities count " <<  m_priorMultiplicityContent.size() << " is too small: please correct through job properties.");
      return InDet::PixelClusterSplitProb(std::vector<double>());
    }

    double psum=0;
    int count=0;
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter,++count)
    {
      psum+=(*iter)/m_priorMultiplicityContent[count];
    }

    count=0;
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter,++count)
    {
      (*iter)/=m_priorMultiplicityContent[count];
      (*iter)/=psum;
    }

    double sumTest=0;

    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      ATH_MSG_VERBOSE("After update prob is: " << *iter);
      sumTest+=*iter;
    }

    ATH_MSG_VERBOSE(" Sum of cluster probabilities is: "<<sumTest);
    for (std::vector<double>::iterator iter=begin;iter!=end;++iter)
    {
      if (iter!=begin)
      {
        vectorOfSplitProbs.push_back(*iter);
      }
    }

    ATH_MSG_VERBOSE(" normalized P(1->2): " << vectorOfSplitProbs[0] << " P(2->3): " << vectorOfSplitProbs[1] );

    PixelClusterSplitProb  clusterSplitProb(vectorOfSplitProbs);

    ATH_MSG_VERBOSE("SplitProb: " << clusterSplitProb.splitProbability(2) << " -->3 " << clusterSplitProb.splitProbability(3) );
    
    return clusterSplitProb;
  }



  
  
}//end namespace



