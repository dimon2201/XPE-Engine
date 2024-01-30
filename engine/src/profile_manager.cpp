#include <profile_manager.hpp>

namespace xpe
{
    namespace core
    {
        bool MProfile::Enabled = false;
        std::ofstream MProfile::s_File;

        void MProfile::BeginSession()
        {
            Enabled = true;

            if (!MFile::Exists("profiles"))
            {
                MFile::CreateDir("profiles");
            }

            auto time = std::time(nullptr);
            auto localTime = *std::localtime(&time);
            std::stringstream ss;
            ss << "profiles/";
            ss << "Profile_";
            ss << std::put_time(&localTime, "%d-%m-%Y_%H-%M-%S");
            ss << ".json";

            s_File.open(ss.str());

            s_File << "{\"otherData\": {},\"traceEvents\":[";
        }

        void MProfile::EndSession()
        {
            Enabled = false;
            s_File << "{";
            s_File << "\"cat\":\"function\",";
            s_File << "\"dur\":0,";
            s_File << "\"name\":\"EndSession\",";
            s_File << "\"ph\":\"X\",";
            s_File << "\"pid\":0,";
            s_File << "\"tid\":0,";
            s_File << "\"ts\":0";
            s_File << "}]}";
            s_File.flush();
            s_File.close();
        }

        void MProfile::Write(const sProfile& profile)
        {
            std::stringstream ss;
            ss << profile.Function << " (" << profile.File << ":" << profile.Line << ")";
            std::string name = ss.str();
            ss = {};
            std::replace(name.begin(), name.end(), '"', '\'');
            long long duration = profile.EndTime - profile.BeginTime;
            u32 pid = MHardware::GetCurrentPID();
            u32 tid = cThread::GetCurrentID();
            ss << "{";
            ss << "\"cat\":\"function\",";
            ss << "\"dur\":" << duration << ',';
            ss << "\"name\":\"" << name << "\",";
            ss << "\"ph\":\"X\",";
            ss << "\"pid\":" << pid << ",";
            ss << "\"tid\":" << tid << ",";
            ss << "\"ts\":" << profile.BeginTime;
            ss << "}";
            ss << ",";
            s_File << ss.str();
            s_File.flush();
        }

        cProfileTimer::cProfileTimer(const char* function, const char* file, int line)
        {
            if (MProfile::Enabled)
            {
                m_Profile.Function = function;
                m_Profile.File = file;
                m_Profile.Line = line;
                m_Profile.BeginTime = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();
            }
        }

        cProfileTimer::~cProfileTimer()
        {
            if (MProfile::Enabled)
            {
                m_Profile.EndTime = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();
                MProfile::Write(m_Profile);
            }
        }
    }
}