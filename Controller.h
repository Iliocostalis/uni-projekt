

class Controller
{
    Controller();
    
public:
    Controller(Controller const&)      = delete;
    void operator=(Controller const&)  = delete;

    static Controller* getInstance();

    void start();
    void stop();
};