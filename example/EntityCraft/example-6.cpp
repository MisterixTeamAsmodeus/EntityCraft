#include <EntityCraft/entitycraft.h>
#include <SqliteAdapter/sqliteadapter.hpp>

#include <ostream>
#include <string>

struct Answer
{
    int id = 0;
    int parentId = 0;
    std::string text = {};
};

struct AnswerTable
{
    static constexpr auto id = "id";
    static constexpr auto parentId = "parentId";
    static constexpr auto text = "text";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<Answer>("", "Answer",
            make_column(id, &Answer::id, query_craft::primary_key()),
            make_column(parentId, &Answer::parentId, query_craft::not_null()),
            make_column(text, &Answer::text));
    }
};

struct CorrectAnswer
{
    int id = 0;
    int parentId = 0;
    int correctAnswerId = 0;
};

struct CorrectAnswerTable
{
    static constexpr auto id = "id";
    static constexpr auto parentId = "parentId";
    static constexpr auto correctAnswerId = "correctAnswerId";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<CorrectAnswer>("", "CorrectAnswer",
            make_column(id, &CorrectAnswer::id, query_craft::primary_key()),
            make_column(parentId, &CorrectAnswer::parentId, query_craft::not_null()),
            make_column(correctAnswerId, &CorrectAnswer::correctAnswerId, query_craft::not_null()));
    }
};

struct File
{
    int id = 0;
    std::string path = {};
};

struct FileTable
{
    static constexpr auto id = "id";
    static constexpr auto path = "path";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<File>("", "File",
            make_column(id, &File::id, query_craft::primary_key()),
            make_column(path, &File::path, query_craft::not_null()));
    }
};

struct Question
{
    int id = 0;
    int parentId = 0;
    std::string text;
    File file;
    /// Список ответов на вопрос
    std::vector<Answer> answers;
    /// Список идентификаторов правильных ответов
    std::vector<CorrectAnswer> correctAnswer;
};

struct QuestionTable
{
    static constexpr auto id = "id";
    static constexpr auto parentId = "parentId";
    static constexpr auto text = "text";
    static constexpr auto fileId = "fileId";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<Question>("", "Question",
            make_column(id, &Question::id, query_craft::primary_key()),
            make_column(parentId, &Question::parentId, query_craft::not_null()),
            make_column(text, &Question::text, query_craft::not_null()),
            make_reference_column(fileId, &Question::file, FileTable::dto(), relation_type::one_to_one),
            make_reference_column(AnswerTable::parentId, &Question::answers, AnswerTable::dto(), relation_type::one_to_many),
            make_reference_column(CorrectAnswerTable::parentId, &Question::correctAnswer, CorrectAnswerTable::dto(), relation_type::one_to_many));
    }
};

struct Examination
{
    int id;
    std::string name;
    std::string description;

    std::vector<Question> questions;
};

struct ExaminationTable
{
    static constexpr auto id = "id";
    static constexpr auto name = "name";
    static constexpr auto description = "description";

    static auto dto()
    {
        using namespace entity_craft;
        return make_table<Examination>("", "Examination",
            make_column(id, &Examination::id, query_craft::primary_key()),
            make_column(name, &Examination::name, query_craft::not_null()),
            make_column(description, &Examination::description),
            make_reference_column(QuestionTable::parentId, &Examination::questions, QuestionTable::dto(), relation_type::one_to_many));
    }
};

struct ExaminationStorage
{
    using Storage = storage_type(ExaminationTable::dto());

    explicit ExaminationStorage(const std::shared_ptr<database_adapter::IConnection>& adapter)
        : _storage(make_storage(adapter, ExaminationTable::dto()))
    {
        create_table();
    }

    auto& operator()()
    {
        return _storage;
    }

private:
    void create_table() const
    {
        _storage.database()->exec("CREATE TABLE IF NOT EXISTS File (id INTEGER NOT NULL, \"path\" TEXT NOT NULL, CONSTRAINT File_PK PRIMARY KEY (id));");
        _storage.database()->exec("CREATE TABLE IF NOT EXISTS Examination (id INTEGER NOT NULL, name TEXT NOT NULL, description INTEGER, CONSTRAINT Examination_PK PRIMARY KEY (id));");
        _storage.database()->exec("CREATE TABLE IF NOT EXISTS Question (id INTEGER NOT NULL, parentId INTEGER NOT NULL, \"text\" TEXT NOT NULL, fileId INTEGER, CONSTRAINT Question_PK PRIMARY KEY (id), CONSTRAINT Question_File_FK FOREIGN KEY (fileId) REFERENCES File(id) CONSTRAINT Question_Examination_FK FOREIGN KEY (parentId) REFERENCES Examination(id));");
        _storage.database()->exec("CREATE TABLE IF NOT EXISTS Answer (id INTEGER NOT NULL, parentId INTEGER NOT NULL, text TEXT, CONSTRAINT Answer_PK PRIMARY KEY (id) CONSTRAINT Answer_Question_FK FOREIGN KEY (parentId) REFERENCES Question(id));");
        _storage.database()->exec("CREATE TABLE IF NOT EXISTS CorrectAnswer (id INTEGER NOT NULL, parentId INTEGER NOT NULL, correctAnswerId INTEGER NOT NULL, CONSTRAINT CorrectAnswer_PK PRIMARY KEY (id) CONSTRAINT CorrectAnswer_Question_FK FOREIGN KEY (parentId) REFERENCES Question(id), CONSTRAINT CorrectAnswer_Answer_FK FOREIGN KEY (correctAnswerId) REFERENCES Answer(id));");
    }

private:
    Storage _storage;
};

class Logger final : public database_adapter::ILogger
{
public:
    ~Logger() override = default;

    void log_error(const std::string& message) override
    {
        std::cout << "LOG_ERROR : " << message << "\n";
    }

    void log_sql(const std::string& message) override
    {
        std::cout << "LOG_DEBUG : " << message << "\n";
    }
};

int main()
{
    using namespace entity_craft;

    database_adapter::sqlite::connection::set_logger(std::make_shared<Logger>());

    database_adapter::sqlite::settings settings;
    settings.url = "example-6.db";
    std::remove(settings.url.c_str());

    std::shared_ptr<database_adapter::IConnection> adapter = std::make_shared<database_adapter::sqlite::connection>(settings);

    auto storage = ExaminationStorage(adapter);

    std::cout << "\n";

    Examination exam;
    exam.id = 1;
    exam.name = "exam";

    Question question;
    question.id = 1;
    question.text = "question";
    question.parentId = exam.id;

    exam.questions.emplace_back(question);

    storage().upsert(exam);

    exam.questions.front().file.id = 1;
    exam.questions.front().file.path = "path";

    std::cout << "\n";

    storage().update(exam);

    exam.questions.front().file = {};

    std::cout << "-----------------------------"
              << "\n";

    storage().update(exam);
}
